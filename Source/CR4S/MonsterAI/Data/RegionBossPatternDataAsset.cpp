#include "MonsterAI/Data/RegionBossPatternDataAsset.h"

const TArray<int32>* URegionBossPatternDataAsset::GetSkillSequence(uint8 PatternID, EBossPhase Phase) const
{
	// Find Pattern Data
	const FRegionPatternData* Pattern = PatternList.FindByPredicate(
		[&](const FRegionPatternData& Data)
		{
			return Data.PatternID == PatternID;
		});

	if (!Pattern)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RegionBossPatternDataAsset] PatternID %d not found."), PatternID);
		return nullptr;
	}

	// Find Skill Sequence for Current Phase
	const FPhaseSkillSequence* Sequence = Pattern->PhaseSequences.FindByPredicate(
		[&](const FPhaseSkillSequence& Seq)
		{
			return Seq.Phase == Phase;
		});

	if (!Sequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RegionBossPatternDataAsset] Phase %d not found in Pattern %d."), static_cast<uint8>(Phase), PatternID);
		return nullptr;
	}

	return &Sequence->SkillIndices;
}
