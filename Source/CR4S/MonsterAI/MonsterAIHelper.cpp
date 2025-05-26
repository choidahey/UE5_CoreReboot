#include "MonsterAI/MonsterAIHelper.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MonsterAI/BaseMonster.h"

ABaseMonster* MonsterAIHelper::GetControlledMonster(UBehaviorTreeComponent& OwnerComp)
{
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		return Cast<ABaseMonster>(AIC->GetPawn());
	}

	return nullptr;
}
