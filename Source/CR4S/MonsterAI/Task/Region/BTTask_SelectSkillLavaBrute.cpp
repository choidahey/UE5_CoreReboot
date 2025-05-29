#include "MonsterAI/Task/Region/BTTask_SelectSkillLavaBrute.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/MonsterAIHelper.h"

int32 UBTTask_SelectSkillLavaBrute::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedMonster.IsValid() || !Target) return INDEX_NONE;

	const float Distance = FVector::DistSquared(CachedMonster->GetActorLocation(), Target->GetActorLocation());

	TArray<int32> SkillPool;

	for (int32 SkillID : AvailableSkills)
	{
		if (SkillID == 0)
			SkillPool.Add(SkillID);
		else if (SkillID == 1 && Distance >= 1600)
			SkillPool.Add(SkillID);
		else if (SkillID == 2)
			SkillPool.Add(SkillID);
		else if (SkillID == 3)
			SkillPool.Add(SkillID);
		else if (SkillID == 4)
			SkillPool.Add(SkillID);
		else if ((SkillID == 5 || SkillID == 6) && Distance <= 900)
			SkillPool.Add(SkillID);
	}

	if (!SkillPool.IsEmpty())
	{
		const int32 Index = FMath::RandRange(0, SkillPool.Num() - 1);
		return SkillPool[Index];
	}

	return AvailableSkills[FMath::RandRange(0, AvailableSkills.Num() - 1)];
}
