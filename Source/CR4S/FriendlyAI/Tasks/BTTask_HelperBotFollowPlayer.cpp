#include "BTTask_HelperBotFollowPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_HelperBotFollowPlayer::UBTTask_HelperBotFollowPlayer()
{
    NodeName = TEXT("HelperBot Follow Player (RVO)");
    bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_HelperBotFollowPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }
    
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        return EBTNodeResult::Failed;
    }
    
    AICon->MoveToActor(PlayerPawn, AcceptableRadius);
    
    return EBTNodeResult::Succeeded;
}