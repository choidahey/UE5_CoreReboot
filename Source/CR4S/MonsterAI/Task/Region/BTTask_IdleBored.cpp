#include "MonsterAI/Task/Region/BTTask_IdleBored.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CR4S/MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"

UBTTask_IdleBored::UBTTask_IdleBored()
{
	NodeName = TEXT("PlayIdleBoredMontage");
}

EBTNodeResult::Type UBTTask_IdleBored::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Monster || MontageList.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	if (UMonsterAnimComponent* AnimComp = Monster->FindComponentByClass<UMonsterAnimComponent>())
	{
		if (AnimComp->IsAnyMontagePlaying())
		{
			return EBTNodeResult::Failed;
		}

		const int32 Index = FMath::RandRange(0, MontageList.Num() - 1);
		UAnimMontage* SelectedMontage = MontageList[Index];
		AnimComp->PlayMontange(SelectedMontage);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
