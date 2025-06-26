#include "MonsterAI/Task/Season/BTTask_SetSelectSkillIndex.h"
#include "BTService_UpdateDistanceToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_SetSelectSkillIndex::UBTTask_SetSelectSkillIndex()
{
	NodeName = TEXT("SetSelectSkillIndex");
}

int32 UBTTask_SetSelectSkillIndex::SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target)
{
	if (!CachedBlackboard || AvailableSkills.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Invalid Blackboard or no skills available"), *GetName());
		return INDEX_NONE;
	}
	
	if (AvailableSkills.Contains(SetSkillIndex))
	{
		return SetSkillIndex;
	}

	const int32 RandIdx = FMath::RandRange(0, AvailableSkills.Num() - 1);
	return AvailableSkills[RandIdx];
}