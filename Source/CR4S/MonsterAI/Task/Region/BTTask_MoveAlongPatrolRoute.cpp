#include "MonsterAI/Task/Region/BTTask_MoveAlongPatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "MonsterAI/Region/PatrolRoute.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_MoveAlongPatrolRoute::UBTTask_MoveAlongPatrolRoute()
{
	NodeName = TEXT("MoveAlongPatrolRoute");
}

EBTNodeResult::Type UBTTask_MoveAlongPatrolRoute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ARegionBossMonster* Boss = Cast<ARegionBossMonster>(AIController->GetPawn());
	if (!Boss || !Boss->PatrolRouteActor) return EBTNodeResult::Failed;

	const FVector PatrolLocation = Boss->GetNextPatrolLocation();

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsVector(FRegionBossAIKeys::PatrolTargetLocation, PatrolLocation);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
