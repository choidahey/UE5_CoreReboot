#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SelectSkill_Hiems.generated.h"

struct FSkillWeight
{
	int32 SkillID;
	int32 Weight;
};

UCLASS()
class CR4S_API UBTTask_SelectSkill_Hiems : public UBTTask_SelectSkill
{
	GENERATED_BODY()
	
public:
	UBTTask_SelectSkill_Hiems();
	
	virtual int32 SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target) override;

private:
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float IceRoadForwardTreshold = 1100.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float IceRoadAwayTreshold = 500.f;

	FString MyHeader = TEXT("Task_SelectSkillHiems");
};
