#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WorldTimeManager.generated.h"

class AEnvironmentManager;

USTRUCT(BlueprintType)
struct FWorldTimeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Day = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Minute = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Second = 0;
};

UCLASS()
class CR4S_API UWorldTimeManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FORCEINLINE FWorldTimeData GetCurrentTimeData() const { return CurrentTimeData; }
	FORCEINLINE int64 GetTotalPlayTime() const { return TotalPlayTime; }

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void PauseTime();
	void ResumeTime();
	void ModifyTime(int32 Day, int32 Minute);

protected:

	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void LoadTimeData();

	void UpdateTime();
	void AdvanceSkyTime(int32 Min, int32 Sec);

	void UpdateTimeWidget();

	UPROPERTY(Transient)
	AEnvironmentManager* EnvironmentManager;

	UPROPERTY(EditAnywhere, Category = "WorldTime|Setting")
	int32 WorldTimeMultiplier = 12;
	UPROPERTY(EditAnywhere, Category = "WorldTime|Setting")
	int32 DayLength = 10;
	UPROPERTY(EditAnywhere, Category = "WorldTime|Setting")
	int32 SeasonLength = 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldTime|State", meta = (AllowPrivateAccess = "true"))
	FWorldTimeData CurrentTimeData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldTime|State", meta = (AllowPrivateAccess = "true"))
	int64 TotalPlayTime = 0;

	FTimerHandle TimeUpdateHandle;
};

