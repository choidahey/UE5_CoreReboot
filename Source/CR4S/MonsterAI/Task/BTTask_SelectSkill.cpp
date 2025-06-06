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
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	CachedMonster = Monster;

	UMonsterSkillComponent* SkillComp = CachedMonster->FindComponentByClass<UMonsterSkillComponent>();
	if (!Monster || !SkillComp) return EBTNodeResult::Failed;

	// TEST LOG
	UE_LOG(LogTemp, Warning, TEXT("[UBTTask_SelectSkill] SkillComp: %s @%p (%s)"), *SkillComp->GetName(), SkillComp, *SkillComp->GetOwner()->GetName());

	TArray<int32> AvailableSkills = SkillComp->GetAvailableSkillIndices();
	if (AvailableSkills.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UBTTask_SelectSkill] No skill is available."));
		return EBTNodeResult::Failed;
	}

	if (!OwnerComp.GetBlackboardComponent()) return EBTNodeResult::Failed;
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FAIKeys::TargetActor));
	
	const int32 SelectedSkillIndex = SelectSkillFromAvailable(AvailableSkills, Target);
	UE_LOG(LogTemp, Warning, TEXT("[UBTTask_SelectSkill] SelectedSkill Index is %d"), SelectedSkillIndex);

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(SkillIndex.SelectedKeyName, SelectedSkillIndex);
	
	return EBTNodeResult::Succeeded;
}

int32 UBTTask_SelectSkill::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	return int32();
}
