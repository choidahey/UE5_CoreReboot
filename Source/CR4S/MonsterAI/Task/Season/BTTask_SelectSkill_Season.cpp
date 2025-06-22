#include "MonsterAI/Task/Season/BTTask_SelectSkill_Season.h"
#include "BTService_UpdateDistanceToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_SelectSkill_Season::UBTTask_SelectSkill_Season()
{
	NodeName = TEXT("UBTTask_SelectSkill_Season");
	bCreateNodeInstance = true;

	SkillIndex.SelectedKeyName = FAIKeys::SkillIndex;
	AttackTypeKey.SelectedKeyName = FSeasonBossAIKeys::AttackType;
	CurrentSkillRangeKey.SelectedKeyName = FSeasonBossAIKeys::CurrentSkillRange;
}

EBTNodeResult::Type UBTTask_SelectSkill_Season::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	CachedMonster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	CachedBlackboard = OwnerComp.GetBlackboardComponent();

	if (!CachedMonster.IsValid() || !CachedBlackboard)
		return EBTNodeResult::Failed;

	UMonsterSkillComponent* SkillComp = CachedMonster->FindComponentByClass<UMonsterSkillComponent>();
	if (!IsValid(SkillComp)) 
		return EBTNodeResult::Failed;
	
	TArray<int32> Available = SkillComp->GetAvailableSkillIndices();
	if (Available.IsEmpty())
		return EBTNodeResult::Failed;
	
	AActor* Target = Cast<AActor>(CachedBlackboard->GetValueAsObject(FAIKeys::TargetActor));
	const int32 ChosenIndex = SelectSkillFromAvailable(Available, Target);
	if (ChosenIndex == INDEX_NONE)
		return EBTNodeResult::Failed;

	// Blackboard에 선택된 스킬 인덱스 기록
	CachedBlackboard->SetValueAsInt(SkillIndex.SelectedKeyName, ChosenIndex);

	// MonsterSkillComponent에서 Range 가져와서 Blackboard에 기록
	const float ChosenRange = SkillComp->GetSkillRange(ChosenIndex);
	CachedBlackboard->SetValueAsFloat(CurrentSkillRangeKey.SelectedKeyName, ChosenRange);

	return EBTNodeResult::Succeeded;
}

int32 UBTTask_SelectSkill_Season::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	const int32 Raw = CachedBlackboard->GetValueAsEnum(AttackTypeKey.SelectedKeyName);
	const ESkillAttackType AttackType = static_cast<ESkillAttackType>(Raw);
	
	UMonsterSkillComponent* SkillComp = CachedMonster->FindComponentByClass<UMonsterSkillComponent>();

	TArray<int32> Filtered;
	for (int32 Idx : AvailableSkills)
	{
		const float MeleeOptionalDistance = CachedBlackboard->GetValueAsInt(FSeasonBossAIKeys::MeleeOptimalDistance);
		const float RangedOptionalDistance = CachedBlackboard->GetValueAsInt(FSeasonBossAIKeys::RangedOptimalDistance);
		const float SkillRange = SkillComp->GetSkillRange(Idx);
		
		if (AttackType == ESkillAttackType::Melee)
		{
			if (SkillRange <= MeleeOptionalDistance)
				Filtered.Add(Idx);
		}
		else
		{
			if (SkillRange > RangedOptionalDistance)
				Filtered.Add(Idx);
		}
	}
	
	if (Filtered.IsEmpty())
		Filtered = AvailableSkills;
	
	const int32 Prev = CachedBlackboard->GetValueAsInt(
		SkillIndex.SelectedKeyName);
	
	Filtered.Remove(Prev);
	
	if (Filtered.IsEmpty())
		Filtered = AvailableSkills;
	
	const int32 RandPos = UKismetMathLibrary::RandomIntegerInRange(0, Filtered.Num() - 1);
	return Filtered[RandPos];
}