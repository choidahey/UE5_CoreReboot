#include "MonsterAI/Task/Region/BTTask_MoveToBaseCampLocation.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"

UBTTask_MoveToBaseCampLocation::UBTTask_MoveToBaseCampLocation()
{
	NodeName = TEXT("MoveToBaseCampLocation");
}

EBTNodeResult::Type UBTTask_MoveToBaseCampLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
    if (!Monster || !Monster->GetAnimComponent() || !ReturnMontage)
    {
        return EBTNodeResult::Failed;
    }

    if (UMonsterAnimComponent* AnimComp = Monster->GetAnimComponent())
    {
        AnimComp->PlayMontange(ReturnMontage);
    }

	return EBTNodeResult::Type();
}
