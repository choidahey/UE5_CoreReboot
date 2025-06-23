#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SelectSkill_Hiems.generated.h"

UENUM(BlueprintType)
enum class EHiemsSkill : uint8
{
	None = 255	UMETA(DisplayName = "None"),
	Blizzard = 0 UMETA(DisplayName = "Blizzard"),  // 근거리
	IcicleWave = 1 UMETA(DisplayName = "IcicleWave"),  // 근거리
	IceSpike = 2 UMETA(DisplayName = "IceSpike"),  // 원거리
	ColdFairy = 3 UMETA(DisplayName = "ColdFairy"),  // 중거리(일단 원거리로 취급)
	IceFlower = 4 UMETA(DisplayName = "IceFlower"),  // 근거리
	IceRain = 5 UMETA(DisplayName = "IceRain"),  // 중거리(일단 원거리로 취급)
	IceRoadAway = 6 UMETA(DisplayName = "IceRoadAway"), // 이동기
	IceRoadForward = 7 UMETA(DisplayName = "IceRoadForward"),  // 이동기
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
	int32 PreviousSkillIndex = INDEX_NONE;

	float DistanceAwayIceRoad = 1000.f;
	float DistanceForwordIceRoad = 4500.f;
	
	FString MyHeader = TEXT("Task_SelectSkillHiems");
};
