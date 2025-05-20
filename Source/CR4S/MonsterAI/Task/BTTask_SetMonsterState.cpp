#include "MonsterAI/Task/BTTask_SetMonsterState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTTask_SetMonsterState::UBTTask_SetMonsterState()
{
	NodeName = TEXT("SetMonsterState");
}

EBTNodeResult::Type UBTTask_SetMonsterState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BTTask_SetMonsterStateFromBB] Blackboard is null"));
		return EBTNodeResult::Failed;
	}

	int32 StateValue = BB->GetValueAsInt(StateKey.SelectedKeyName);
	EMonsterState TargetState = static_cast<EMonsterState>(StateValue);

	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	if (UMonsterStateComponent* StateComp = Monster->GetStateComponent())
	{
		StateComp->SetState(TargetState);
		UE_LOG(LogTemp, Log, TEXT("[BTTask_SetMonsterStateFromBB] State set to %s"), *UEnum::GetValueAsString(TargetState));
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Type();
}
