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
        UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
        if (QueryInstance)
        {
            QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            QueryInstance = nullptr;
        }
        OwnerCompPtr = nullptr;
        FinishLatentTask(*TempOwnerComp, EBTNodeResult::Failed);
        return;
    }

    TArray<FVector> Locations = Wrapper->GetResultsAsLocations();
    if (Locations.Num() == 0)
    {
        UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
        if (QueryInstance)
        {
            QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            QueryInstance = nullptr;
        }
        OwnerCompPtr = nullptr;
        FinishLatentTask(*TempOwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    FVector RawTarget = Locations[0];
    FNavLocation Projected;
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(OwnerCompPtr->GetWorld());
    FVector Target;
    if (NavSys && NavSys->ProjectPointToNavigation(RawTarget, Projected, FVector(200.f)))
    {
        Target = Projected.Location;
    }
    else
    {
        UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
        if (QueryInstance)
        {
            QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            QueryInstance = nullptr;
        }
        OwnerCompPtr = nullptr;
        FinishLatentTask(*TempOwnerComp, EBTNodeResult::Failed);
        return;
    }

    OwnerCompPtr->GetBlackboardComponent()->SetValueAsVector("ResourceLocation", Target);
    TArray<AActor*> ResultActors = Wrapper->GetResultsAsActors();
    if (ResultActors.Num() > 0)
    {
        OwnerCompPtr->GetBlackboardComponent()->SetValueAsObject("ResourceTarget", ResultActors[0]);

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
        UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
        if (QueryInstance)
        {
            QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            QueryInstance = nullptr;
        }
        OwnerCompPtr = nullptr;
        FinishLatentTask(*TempOwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    if (ACharacter* CharPawn = Cast<ACharacter>(AICon->GetPawn()))
    {
        if (UCharacterMovementComponent* MoveComp = CharPawn->GetCharacterMovement())
        {
            MoveComp->SetAvoidanceEnabled(true);
        }
    }
    
    if (FVector::Dist(AICon->GetPawn()->GetActorLocation(), Target) <= AcceptanceRadius)
    {
        if (QueryInstance)
        {
            QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            QueryInstance = nullptr;
        }
        UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
        OwnerCompPtr = nullptr;
        FinishLatentTask(*TempOwnerComp, EBTNodeResult::Succeeded);
        return;
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
        if (QueryInstance)
        {
            QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            QueryInstance = nullptr;
        }
        return;
    }

    UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
    if (QueryInstance)
    {
        QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
        QueryInstance = nullptr;
    }
    OwnerCompPtr = nullptr;
    FinishLatentTask(*TempOwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_HelperBotBMoveToResource::HandleMoveCompleted(
    FAIRequestID RequestID,
    EPathFollowingResult::Type Result)
{
    if (AAIController* AICon = Cast<AAIController>(OwnerCompPtr->GetAIOwner()))
    {
        AICon->ReceiveMoveCompleted.RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::HandleMoveCompleted);
        
        FVector Target = OwnerCompPtr->GetBlackboardComponent()->GetValueAsVector("ResourceLocation");

        if (APawn* Pawn = AICon->GetPawn())
        {
            FRotator CurrentRot = Pawn->GetActorRotation();
            FRotator LookRot   = (Target - Pawn->GetActorLocation()).Rotation();
            CurrentRot.Yaw     = LookRot.Yaw;
            Pawn->SetActorRotation(CurrentRot);
        }
    }

    EBTNodeResult::Type TaskResult = (Result == EPathFollowingResult::Success) 
    ? EBTNodeResult::Succeeded 
    : EBTNodeResult::Failed;

    UBehaviorTreeComponent* TempOwnerComp = OwnerCompPtr;
    OwnerCompPtr = nullptr;
    FinishLatentTask(*TempOwnerComp, TaskResult);
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToResource::AbortTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    if (QueryInstance)
    {
        QueryInstance->GetOnQueryFinishedEvent()
            .RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
        QueryInstance = nullptr;
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

    OwnerCompPtr = nullptr;
    return EBTNodeResult::Aborted;
}
