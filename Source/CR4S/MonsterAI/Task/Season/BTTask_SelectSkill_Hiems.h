#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SelectSkill_Hiems.generated.h"

UENUM(BlueprintType)
enum class EHiemsSkill : uint8
{
	None = 255	UMETA(DisplayName = "None"),
	Blizzard = 0 UMETA(DisplayName = "Blizzard"),
	IcicleWave = 1 UMETA(DisplayName = "IcicleWave"),
	IceSpike = 2 UMETA(DisplayName = "IceSpike"),
	ColdFairy = 3 UMETA(DisplayName = "ColdFairy"),
	IceRoadAway = 4 UMETA(DisplayName = "IceRoadAv"),
	IceRoadForward = 5 UMETA(DisplayName = "IceRoadFo"),
};

UCLASS()
class CR4S_API UBTTask_SelectSkill_Hiems : public UBTTask_SelectSkill
{
	GENERATED_BODY()
	
public:
	UBTTask_SelectSkill_Hiems();

protected:
	virtual int32 SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target) override;
	
private:
	void ReshuffleSkills(const TArray<int32>& AvailableSkills);

	UPROPERTY()
	TArray<int32> ShuffledSkills;
	int32 CurrentShuffleIndex = 0;

	float DistanceAwayIceRoad = 500.f;
	float DistanceForwordIceRoad = 1100.f;
	
	FString MyHeader = TEXT("Task_SelectSkillHiems");
};
