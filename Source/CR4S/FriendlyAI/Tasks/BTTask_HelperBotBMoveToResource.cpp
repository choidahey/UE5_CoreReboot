#include "BTTask_HelperBotBMoveToResource.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/BaseHelperBot.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Component/AIJumpComponent.h"

UBTTask_HelperBotBMoveToResource::UBTTask_HelperBotBMoveToResource()
{
    NodeName = TEXT("BMoveToResource");
}

uint16 UBTTask_HelperBotBMoveToResource::GetInstanceMemorySize() const
{
    return sizeof(FBTMoveToResourceMemory);
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToResource::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FBTMoveToResourceMemory* MyMemory = reinterpret_cast<FBTMoveToResourceMemory*>(NodeMemory);
    
    if (!ResourceQuery)
    {
        return EBTNodeResult::Failed;
    }

    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon)
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = AICon->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }

    MyMemory->OwnerCompPtr = &OwnerComp;
    
    UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>();
    if (JumpComp)
    {
        MyMemory->JumpComponent = JumpComp;
        JumpComp->ActivateJumpComponent(AcceptanceRadius);
        //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Jump component activated"));
    }
    // else
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Jump component not found!"));
    // }
    
    //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Starting EQS query..."));
    
    MyMemory->QueryInstance = UEnvQueryManager::RunEQSQuery(
        OwnerComp.GetWorld(),
        ResourceQuery,
        AICon,
        EEnvQueryRunMode::SingleResult,
        UEnvQueryInstanceBlueprintWrapper::StaticClass()
    );

    if (!MyMemory->QueryInstance)
    {
        return EBTNodeResult::Failed;
    }

    MyMemory->QueryInstance->GetOnQueryFinishedEvent()
        .AddDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);

    return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotBMoveToResource::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* Wrapper, EEnvQueryStatus::Type Status)
{
    if (!IsValid(Wrapper))
    {
        return;
    }
    
    FBTMoveToResourceMemory* MyMemory = nullptr;
    
    for (TObjectIterator<UBehaviorTreeComponent> It; It; ++It)
    {
        UBehaviorTreeComponent* BTComp = *It;
        if (!BTComp || !IsValid(BTComp))
        {
            continue;
        }
        
        uint8* NodeMem = BTComp->GetNodeMemory(this, BTComp->GetActiveInstanceIdx());
        if (!NodeMem)
        {
            continue;
        }
        
        FBTMoveToResourceMemory* TempMemory = reinterpret_cast<FBTMoveToResourceMemory*>(NodeMem);
        if (!TempMemory)
        {
            continue;
        }
        
        if (TempMemory->QueryInstance == Wrapper)
        {
            MyMemory = TempMemory;
            break;
        }
    }
    
    if (!MyMemory || Status != EEnvQueryStatus::Success)
    {
        //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Query failed or memory not found"));
        if (MyMemory && MyMemory->OwnerCompPtr)
        {
            if (MyMemory->QueryInstance && MyMemory->QueryInstance->IsValidLowLevel())
            {
                MyMemory->QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
                MyMemory->QueryInstance = nullptr;
            }
            if (MyMemory->JumpComponent)
            {
                MyMemory->JumpComponent->DeactivateJumpComponent();
            }
            FinishLatentTask(*MyMemory->OwnerCompPtr, EBTNodeResult::Failed);
        }
        return;
    }

    TArray<AActor*> ResultActors = Wrapper->GetResultsAsActors();
    if (ResultActors.Num() == 0)
    {
        //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: No actors found in EQS results"));
        if (MyMemory->QueryInstance && MyMemory->QueryInstance->IsValidLowLevel())
        {
            MyMemory->QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
            MyMemory->QueryInstance = nullptr;
        }
        if (MyMemory->JumpComponent)
        {
            MyMemory->JumpComponent->DeactivateJumpComponent();
        }
        FinishLatentTask(*MyMemory->OwnerCompPtr, EBTNodeResult::Failed);
        return;
    }
    
    AActor* TargetActor = ResultActors[0];
    // UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: EQS Result Actor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));
    // UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: EQS Result Location: %s"), *TargetActor->GetActorLocation().ToString());

    FVector AdjustedLocation = TargetActor->GetActorLocation();
    AdjustedLocation.Z += 10.0f;
    TargetActor->SetActorLocation(AdjustedLocation);
    
    MyMemory->OwnerCompPtr->GetBlackboardComponent()->SetValueAsObject("TargetActor", TargetActor);
    //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: TargetActor set in blackboard"));

    if (ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(MyMemory->OwnerCompPtr->GetAIOwner()->GetPawn()))
    {
        if (HelperBot->WorkTargetParticle)
        {
            FVector Origin, BoxExtent;
            TargetActor->GetActorBounds(false, Origin, BoxExtent);
            FVector ParticleLocation = FVector(Origin.X, Origin.Y, Origin.Z - BoxExtent.Z);

            HelperBot->WorkTargetParticle->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            HelperBot->WorkTargetParticle->SetWorldLocation(ParticleLocation);
            HelperBot->WorkTargetParticle->Activate(true);
            
            //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Work particle activated at: %s"), *ParticleLocation.ToString());
        }
    }

    if (MyMemory->QueryInstance && MyMemory->QueryInstance->IsValidLowLevel())
    {
        MyMemory->QueryInstance->GetOnQueryFinishedEvent().RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
        MyMemory->QueryInstance = nullptr;
    }

    FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UBTTask_HelperBotBMoveToResource::CheckReachedTarget, MyMemory);
    MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().SetTimer(MyMemory->CheckTimer, TimerDelegate, 0.5f, true);
    
    //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Query finished successfully, jump component should now move to target"));
}

void UBTTask_HelperBotBMoveToResource::CheckReachedTarget(FBTMoveToResourceMemory* MyMemory)
{
    if (!MyMemory || !MyMemory->JumpComponent || !MyMemory->OwnerCompPtr)
        return;

    bool bReached = MyMemory->JumpComponent->HasReachedTarget();
    //UE_LOG(LogTemp, Warning, TEXT("CheckReachedTarget: HasReachedTarget = %s"), bReached ? TEXT("TRUE") : TEXT("FALSE"));

    if (bReached)
    {
        MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->CheckTimer);
        MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->RotationTimer);
        MyMemory->JumpComponent->DeactivateJumpComponent();

        if (ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(MyMemory->OwnerCompPtr->GetAIOwner()->GetPawn()))
        {
            if (HelperBot->WorkTargetParticle)
            {
                HelperBot->WorkTargetParticle->Deactivate();
            }
        }

        HandleTargetRotation(MyMemory);
    
        FinishLatentTask(*MyMemory->OwnerCompPtr, EBTNodeResult::Succeeded);
    }
}

void UBTTask_HelperBotBMoveToResource::HandleTargetRotation(FBTMoveToResourceMemory* MyMemory)
{
    if (!MyMemory || !MyMemory->OwnerCompPtr || !IsValid(MyMemory->OwnerCompPtr))
    {
        return;
    }
       
    AAIController* AICon = MyMemory->OwnerCompPtr->GetAIOwner();
    if (!AICon || !IsValid(AICon))
    {
        return;
    }
       
    APawn* Pawn = AICon->GetPawn();
    if (!Pawn || !IsValid(Pawn))
    {
        return;
    }
    
    UBlackboardComponent* BlackboardComp = MyMemory->OwnerCompPtr->GetBlackboardComponent();
    if (!BlackboardComp || !IsValid(BlackboardComp))
    {
        return;
    }
       
    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
    if (!TargetActor || !IsValid(TargetActor))
    {
        return;
    }

    float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetActor->GetActorLocation());
    //UE_LOG(LogTemp, Warning, TEXT("HandleTargetRotation: Distance to target = %.2f"), Distance);
       
    FVector TargetDirection = (TargetActor->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = TargetDirection.Rotation();
    FRotator CurrentRotation = Pawn->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, 0.016f, 270.0f);
    Pawn->SetActorRotation(NewRotation);
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToResource::AbortTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: AbortTask called"));
    
    FBTMoveToResourceMemory* MyMemory = reinterpret_cast<FBTMoveToResourceMemory*>(NodeMemory);
    
    if (MyMemory->QueryInstance)
    {
        MyMemory->QueryInstance->GetOnQueryFinishedEvent()
            .RemoveDynamic(this, &UBTTask_HelperBotBMoveToResource::OnQueryFinished);
        MyMemory->QueryInstance = nullptr;
    }

    if (MyMemory->OwnerCompPtr)
    {
        MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->CheckTimer);
        MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->RotationTimer);
    }

    if (AAIController* AICon = Cast<AAIController>(OwnerComp.GetAIOwner()))
    {
        if (ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(AICon->GetPawn()))
        {
            if (HelperBot->WorkTargetParticle)
            {
                HelperBot->WorkTargetParticle->Deactivate();
            }
        }
    }

    if (MyMemory->JumpComponent)
    {
        MyMemory->JumpComponent->DeactivateJumpComponent();
        MyMemory->JumpComponent = nullptr;
        //UE_LOG(LogTemp, Warning, TEXT("BTTask_HelperBotBMoveToResource: Jump component deactivated"));
    }
    
    MyMemory->OwnerCompPtr = nullptr;
    return EBTNodeResult::Aborted;
}