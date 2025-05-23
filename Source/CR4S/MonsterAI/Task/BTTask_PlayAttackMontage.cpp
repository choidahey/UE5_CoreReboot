#include "MonsterAI/Task/BTTask_PlayAttackMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTTask_PlayAttackMontage::UBTTask_PlayAttackMontage()
{
	NodeName = TEXT("PlayAttackMontage");
	SkillIndexKey.SelectedKeyName = FAIKeys::SkillIndex;
}

EBTNodeResult::Type UBTTask_PlayAttackMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Monster) return EBTNodeResult::Failed;

	UMonsterSkillComponent* SkillComp = Monster->FindComponentByClass<UMonsterSkillComponent>();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!SkillComp || !BlackboardComp) return EBTNodeResult::Failed;

	const int32 SkillIndex = BlackboardComp->GetValueAsInt(SkillIndexKey.SelectedKeyName);

	if (!SkillComp->IsSkillReady(SkillIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UBTTask_PlayAttackMontage] Skill %d is not ready."), SkillIndex);
		return EBTNodeResult::Failed;
	}

	SkillComp->UseSkill(SkillIndex);

	UE_LOG(LogTemp, Log, TEXT("[UBTTask_PlayAttackMontage] Used SkillID: %d"), SkillIndex);
	return EBTNodeResult::Succeeded;
}
