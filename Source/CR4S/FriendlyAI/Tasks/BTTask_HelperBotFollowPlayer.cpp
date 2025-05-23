#include "BTTask_HelperBotFollowPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_HelperBotFollowPlayer::UBTTask_HelperBotFollowPlayer()
{
    NodeName = TEXT("HelperBot Follow Player (RVO)");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HelperBotFollowPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotFollowPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;
    AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!AIController || !AIPawn || !PlayerPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (ACharacter* CharPawn = Cast<ACharacter>(AIPawn))
    {
        if (UCharacterMovementComponent* MoveComp = CharPawn->GetCharacterMovement())
        {
            MoveComp->SetAvoidanceEnabled(true);
        }
    }

    const float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), AIPawn->GetActorLocation());
    const float StartFollowDistance = 300.0f;
    const float StopFollowDistance = 150.0f;

    if (Distance > StartFollowDistance)
    {
        FAIMoveRequest MoveReq;
        MoveReq.SetGoalActor(PlayerPawn);
        MoveReq.SetAcceptanceRadius(StopFollowDistance);
        AIController->MoveTo(MoveReq);
    }
    else if (Distance <= StopFollowDistance)
    {
        if (AIController->GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Moving)
        {
            AIController->StopMovement();
        }
    }
}