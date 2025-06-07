#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SelectSkill_Hiems.generated.h"

UCLASS()
class CR4S_API UBTTask_SelectSkill_Hiems : public UBTTask_SelectSkill
{
	GENERATED_BODY()
	
public:
	UBTTask_SelectSkill_Hiems();
	
	virtual int32 SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Task)
	float IceRoadForwardTreshold = 1100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Task)
	float IceRoadAwayTreshold = 500.f;
	
private:
	void ReshuffleSkills(const TArray<int32>& AvailableSkills);

	UPROPERTY()
	TArray<int32> ShuffledSkills;
	int32 CurrentShuffleIndex = 0;
	
	FString MyHeader = TEXT("Task_SelectSkillHiems");
};
