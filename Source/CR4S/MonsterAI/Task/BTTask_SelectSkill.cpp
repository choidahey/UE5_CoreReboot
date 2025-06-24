#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "AIController.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

EBTNodeResult::Type UBTTask_SelectSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedMonster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	CachedBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CachedMonster.IsValid() || !CachedBlackboard) return EBTNodeResult::Failed;

	UMonsterSkillComponent* SkillComp = CachedMonster->FindComponentByClass<UMonsterSkillComponent>();

	TArray<int32> AvailableSkills = SkillComp->GetAvailableSkillIndices();
	if (AvailableSkills.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UBTTask_SelectSkill] No skill is available."));
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(CachedBlackboard->GetValueAsObject(FAIKeys::TargetActor));
	const int32 SelectedSkillIndex = SelectSkillFromAvailable(AvailableSkills, Target);
	UE_LOG(LogTemp, Warning, TEXT("[UBTTask_SelectSkill] SelectedSkill Index is %d"), SelectedSkillIndex);

	if (SelectedSkillIndex == INDEX_NONE) return EBTNodeResult::Failed;
	const ESkillAttackType SelectedAttackType = SkillComp->GetSkillData(SelectedSkillIndex).AttackType;

	CachedBlackboard->SetValueAsInt(SkillIndex.SelectedKeyName, SelectedSkillIndex);
	CachedBlackboard->SetValueAsEnum(FRegionBossAIKeys::AttackType, static_cast<uint8>(SelectedAttackType));

	return EBTNodeResult::Succeeded;
}

int32 UBTTask_SelectSkill::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	return int32();
}
