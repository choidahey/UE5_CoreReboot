#include "TestBTT_GFMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "../Component/FlyingMovementComponent.h"
#include "../Component/GroundMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UTestBTT_GFMove::UTestBTT_GFMove()
{
    NodeName     = TEXT("GFMove");
    bNotifyTick  = true;

    TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UTestBTT_GFMove, TargetLocationKey));
}

EBTNodeResult::Type UTestBTT_GFMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                 uint8* )
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return EBTNodeResult::Failed;

    ACharacter* Character = Cast<ACharacter>(Controller->GetPawn());
    if (!Character) return EBTNodeResult::Failed;

    const FVector Dest = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName);
    if (!Dest.IsNearlyZero())
    {
        if (UFlyingMovementComponent* FlyComp = Character->FindComponentByClass<UFlyingMovementComponent>())
        {
            FlyComp->MoveToLocation(Dest);
            bIsFlying = true;
            return EBTNodeResult::InProgress;
        }
        
        if (UGroundMovementComponent* GroundComp = Character->FindComponentByClass<UGroundMovementComponent>())
        {
            GroundComp->MoveToLocation(Dest);
            bIsFlying = false;
            return EBTNodeResult::InProgress;
        }
    }
    return EBTNodeResult::Failed;
}

void UTestBTT_GFMove::TickTask(UBehaviorTreeComponent& OwnerComp,
                               uint8* ,
                               float )
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    if (!Controller) return;

    ACharacter* Character = Cast<ACharacter>(Controller->GetPawn());
    if (!Character) return;
    
    bool bDone = bIsFlying ? IsFlightFinished(Character) : IsGroundMoveFinished(Controller);

    if (bDone)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

bool UTestBTT_GFMove::IsFlightFinished(ACharacter* Character) const
{
    if (UFlyingMovementComponent* FlyComp = Character->FindComponentByClass<UFlyingMovementComponent>())
    {
        const bool bTimerActive = FlyComp->GetWorld()->GetTimerManager().IsTimerActive(
            FlyComp->GetFlightTimerHandle());
        const bool bWalking = Character->GetCharacterMovement()->MovementMode == MOVE_Walking;

        return !bTimerActive && bWalking;
    }
    return false;
}

bool UTestBTT_GFMove::IsGroundMoveFinished(AAIController* Controller) const
{
    EPathFollowingStatus::Type Status = Controller->GetPathFollowingComponent()->GetStatus();
    return Status == EPathFollowingStatus::Type::Idle;
}
