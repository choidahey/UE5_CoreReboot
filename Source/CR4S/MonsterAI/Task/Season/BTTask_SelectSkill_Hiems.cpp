#include "MonsterAI/Task/Season/BTTask_SelectSkill_Hiems.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/MonsterAIHelper.h"

int32 UBTTask_SelectSkill_Hiems::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedMonster.IsValid() || !Target) return INDEX_NONE;

	const float Distance = FVector::Dist(CachedMonster->GetActorLocation(), Target->GetActorLocation());
	
	TArray<FSkillWeight> Weights;
	Weights.Reserve(AvailableSkills.Num());

    for (int32 SkillID : AvailableSkills)
    {
        int32 Weight = 1;

        if (Distance >= 1100.f && (SkillID == 3 || SkillID == 5))
        {
            Weight = 5;
        }
        else if (Distance >= 500.f && Distance < 1100.f && SkillID == 2)
        {
            Weight = 5;
        }
        else if (Distance < 500.f && (SkillID == 5 || SkillID == 3))
        {
            Weight = 5;
        }

        Weights.Add({ SkillID, Weight });
    }

    // Save Weight
    int32 TotalWeight = 0;
    for (const FSkillWeight& SkillWeight : Weights)
    {
        TotalWeight += SkillWeight.Weight;
    }
    if (TotalWeight <= 0)
        return INDEX_NONE;

    // return random skill index
    int32 RandomIndex = FMath::RandRange(1, TotalWeight);
    for (const FSkillWeight& SkillWeight : Weights)
    {
        RandomIndex -= SkillWeight.Weight;
        if (RandomIndex <= 0)
        {
            return SkillWeight.SkillID;
        }
    }

    return INDEX_NONE;
}
