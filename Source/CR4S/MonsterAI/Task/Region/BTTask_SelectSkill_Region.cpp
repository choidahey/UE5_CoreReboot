#include "MonsterAI/Task/Region/BTTask_SelectSkill_Region.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Data/RegionBossPatternDataAsset.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

struct FCandidate
{
	int32 SkillIndex;
	float Weight;
};

int32 UBTTask_SelectSkill_Region::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedMonster.IsValid() || !CachedBlackboard) return INDEX_NONE;

	const uint8 PatternID = CachedBlackboard->GetValueAsInt(FRegionBossAIKeys::CurrentPatternID);
	const uint8 PhaseValue = CachedBlackboard->GetValueAsInt(FRegionBossAIKeys::CurrentPhase);
	const int32 StepIndex = CachedBlackboard->GetValueAsInt(FRegionBossAIKeys::PatternStepIndex);

	ARegionBossMonster* RegionBoss = Cast<ARegionBossMonster>(CachedMonster.Get());
	if (!IsValid(RegionBoss) || !RegionBoss->PatternDataAsset) return INDEX_NONE;

	const FRegionPatternData* Pattern = RegionBoss->PatternDataAsset->PatternList.FindByPredicate(
		[&](const FRegionPatternData& P) { return P.PatternID == PatternID; });

	const FPhaseSkillSequence* Sequence = Pattern
		? Pattern->PhaseSequences.FindByPredicate([&](const FPhaseSkillSequence& Seq)
			{ return Seq.Phase == static_cast<EBossPhase>(PhaseValue); })
		: nullptr;

	if (!Sequence || !Sequence->SkillIndices.IsValidIndex(StepIndex))
		return INDEX_NONE;

	const int32 DesiredSkill = Sequence->SkillIndices[StepIndex];
	return AvailableSkills.Contains(DesiredSkill) ? DesiredSkill : INDEX_NONE;
}
