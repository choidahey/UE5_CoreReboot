#include "BTTask_HelperBotBMoveToResource.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/BaseHelperBot.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_HelperBotBMoveToResource::UBTTask_HelperBotBMoveToResource()
{
    NodeName = TEXT("BMoveToResource");
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToResource::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!ResourceQuery)
        return EBTNodeResult::Failed;

    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon)
        return EBTNodeResult::Failed;

    OwnerCompPtr = &OwnerComp;
    
    QueryInstance = UEnvQueryManager::RunEQSQuery(
        OwnerComp.GetWorld(),
        ResourceQuery,
        AICon,
        EEnvQueryRunMode::SingleResult,
        UEnvQueryInstanceBlueprintWrapper::StaticClass()
    );

    if (!QueryInstance)
        return EBTNodeResult::Failed;

    QueryInstance->GetOnQueryFinishedEvent()
        .AddDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);

    return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotBMoveToResource::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* Wrapper, EEnvQueryStatus::Type Status)
{
    if (Status != EEnvQueryStatus::Success || !OwnerCompPtr)
    {
        FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
        return;
    }

    TArray<FVector> Locations = Wrapper->GetResultsAsLocations();
    if (Locations.Num() == 0)
    {
        FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
        return;
    }
    
    FVector RawTarget = Locations[0];
    FNavLocation Projected;
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FVector Target;
    if (NavSys && NavSys->ProjectPointToNavigation(RawTarget, Projected, FVector(200.f)))
    {
        Target = Projected.Location;
    }
    else
    {
        FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
        return;
    }

    OwnerCompPtr->GetBlackboardComponent()->SetValueAsVector(ResourceLocationKey.SelectedKeyName, Target);
    TArray<AActor*> ResultActors = Wrapper->GetResultsAsActors();
    if (ResultActors.Num() > 0)
    {
        OwnerCompPtr->GetBlackboardComponent()
            ->SetValueAsObject(ResourceTargetKey.SelectedKeyName, ResultActors[0]);

        if (ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(OwnerCompPtr->GetAIOwner()->GetPawn()))
        {
            if (HelperBot->WorkTargetParticle)
            {
                FVector Origin, BoxExtent;
                ResultActors[0]->GetActorBounds(false, Origin, BoxExtent);
                FVector ParticleLocation = FVector(Origin.X, Origin.Y, Origin.Z - BoxExtent.Z);

                HelperBot->WorkTargetParticle->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
                HelperBot->WorkTargetParticle->SetWorldLocation(ParticleLocation);
                HelperBot->WorkTargetParticle->Activate(true);
            }
        }
    }
    
    AAIController* AICon = Cast<AAIController>(OwnerCompPtr->GetAIOwner());
    if (!AICon)
    {
        FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
        return;
    }
    
    if (ACharacter* CharPawn = Cast<ACharacter>(AICon->GetPawn()))
    {
        if (UCharacterMovementComponent* MoveComp = CharPawn->GetCharacterMovement())
        {
            MoveComp->SetAvoidanceEnabled(true);
        }
    }
    
    FAIMoveRequest MoveReq(Target);
    MoveReq.SetAcceptanceRadius(AcceptanceRadius);
    MoveReq.SetAllowPartialPath(true);
    FNavPathSharedPtr OutPath;
    EPathFollowingRequestResult::Type Result =
        AICon->MoveTo(MoveReq, &OutPath);

    if (Result == EPathFollowingRequestResult::RequestSuccessful)
    {
        AICon->ReceiveMoveCompleted.AddDynamic(this, &UBTTask_HelperBotBMoveToResource::HandleMoveCompleted);
        return;
    }

    FinishLatentTask(*OwnerCompPtr, EBTNodeResult::Failed);
}

void UBTTask_HelperBotBMoveToResource::HandleMoveCompleted(
    FAIRequestID RequestID,
    EPathFollowingResult::Type Result)
{
    if (AAIController* AICon = Cast<AAIController>(OwnerCompPtr->GetAIOwner()))
    {
        AICon->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::HandleMoveCompleted);
        
        FVector Target = OwnerCompPtr->GetBlackboardComponent()
            ->GetValueAsVector(ResourceLocationKey.SelectedKeyName);

        if (APawn* Pawn = AICon->GetPawn())
        {
            FRotator CurrentRot = Pawn->GetActorRotation();
            FRotator LookRot   = (Target - Pawn->GetActorLocation()).Rotation();
            CurrentRot.Yaw     = LookRot.Yaw;
            Pawn->SetActorRotation(CurrentRot);
        }
    }

    FinishLatentTask(
        *OwnerCompPtr,
        (Result == EPathFollowingResult::Success)
            ? EBTNodeResult::Succeeded
            : EBTNodeResult::Failed
    );
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToResource::AbortTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    if (QueryInstance)
    {
        QueryInstance->GetOnQueryFinishedEvent()
            .RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
    }
    
    if (AAIController* AICon = Cast<AAIController>(OwnerComp.GetAIOwner()))
    {
        AICon->StopMovement();
        AICon->ReceiveMoveCompleted
            .RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::HandleMoveCompleted);
        
        if (ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(AICon->GetPawn()))
        {
            if (HelperBot->WorkTargetParticle)
            {
                HelperBot->WorkTargetParticle->Deactivate();
            }
        }
    }

    return EBTNodeResult::Aborted;
}
