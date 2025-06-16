#include "BTT_FAFlyAway.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FAFlyAway::UBTT_FAFlyAway()
{
	NodeName = TEXT("Fly: Fly Away");
}

EBTNodeResult::Type UBTT_FAFlyAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AICon = OwnerComp.GetAIOwner())
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsBool(TEXT("FlyAway"), bFlyAway);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
