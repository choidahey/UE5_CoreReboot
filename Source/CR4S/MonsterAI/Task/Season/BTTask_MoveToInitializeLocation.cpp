#include "MonsterAI/Task/Season/BTTask_MoveToInitializeLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_MoveToInitializeLocation::UBTTask_MoveToInitializeLocation()
{
	NodeName = TEXT("MoveToInitializeTargetLocation");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_MoveToInitializeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!BB || !AIC) return EBTNodeResult::Failed;

	const FVector InitializeLocation = BB->GetValueAsVector(FSeasonBossAIKeys::InitializeTargetLocation);
	MoveRequestID = AIC->MoveToLocation(InitializeLocation, 5.0f, false);

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToInitializeLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UPathFollowingComponent* PathComp = AIC->GetPathFollowingComponent();
	if (PathComp->GetStatus() != EPathFollowingStatus::Moving)
	{
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (BB)
		{
			BB->ClearValue(FSeasonBossAIKeys::InitializeTargetLocation);
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
