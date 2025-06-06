#include "MonsterAI/Task/Region/BTTask_SelectSkill_Region.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

struct FCandidate
{
	int32 SkillIndex;
	float Weight;
};

int32 UBTTask_SelectSkill_Region::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedMonster.IsValid() || !Target || !CachedBlackboard) return INDEX_NONE;

	const float Distance = FVector::Dist(CachedMonster->GetActorLocation(), Target->GetActorLocation());
	const int32 CurrentPhase = CachedBlackboard->GetValueAsInt(TEXT("CurrentPhase"));

	UMonsterSkillComponent* SkillComp = CachedMonster->FindComponentByClass<UMonsterSkillComponent>();
	if (!SkillComp) return INDEX_NONE;

	UE_LOG(LogTemp, Warning, TEXT("[SelectSkill] Phase: %d, Distance: %.1f"), CurrentPhase, Distance);

	TArray<FCandidate> Candidates;
	float TotalWeight = 0.f;

	for (int32 Index : AvailableSkills)
	{
		const FMonsterSkillData& Skill = SkillComp->GetSkillData(Index);

		for (const FSkillProbabilityEntry& Entry : Skill.ProbabilityList)
		{
			if (Entry.Phase != CurrentPhase) continue;

			if (Distance >= Entry.MinDistance && Distance <= Entry.MaxDistance)
			{
				Candidates.Add({ Index, Entry.Probability });
				TotalWeight += Entry.Probability;

				UE_LOG(LogTemp, Warning,
					TEXT("[SelectSkill] Candidate Skill Index: %d (Distance: %.1f ~ %.1f, Probability: %.2f)"),
					Index, Entry.MinDistance, Entry.MaxDistance, Entry.Probability);

				break;
			}
		}
	}

	if (Candidates.IsEmpty() || TotalWeight <= 0.f)
	{
		const int32 FallbackSkill = AvailableSkills[FMath::RandRange(0, AvailableSkills.Num() - 1)];
		UE_LOG(LogTemp, Warning, TEXT("[SelectSkill] No candidates, fallback to random skill index: %d"), FallbackSkill);
		return FallbackSkill;
	}

	const float Random = FMath::FRandRange(0.f, TotalWeight);
	float Accumulated = 0.f;

	for (const FCandidate& Candidate : Candidates)
	{
		Accumulated += Candidate.Weight;
		if (Random <= Accumulated)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SelectSkill] Selected Skill Index: %d (Random Roll: %.2f / Total: %.2f)"),
				Candidate.SkillIndex, Random, TotalWeight);

			return Candidate.SkillIndex;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[SelectSkill] Selected Skill Index (Last Fallback): %d"), Candidates.Last().SkillIndex);

	return Candidates.Last().SkillIndex;
}
