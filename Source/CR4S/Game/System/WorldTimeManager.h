#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WorldTimeManager.generated.h"

class AEnvironmentManager;
class USeasonManager;

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

	FString ToString() const
	{
		return FString::Printf(TEXT("%02d Days %02d Minutes %02d Seconds"), Day, Minute, Second);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldTimeUpdated, int64, PlayTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMinuteUpdated, int32, Minute, int32, DayCycle);

UCLASS()
class CR4S_API UWorldTimeManager : public UWorldSubsystem
{
	GENERATED_BODY()



#pragma region Getters & Setters
public:
	FORCEINLINE FWorldTimeData GetCurrentTimeData() const { return CurrentTimeData; }
	FORCEINLINE int64 GetTotalPlayTime() const { return TotalPlayTime; }
	FORCEINLINE int32 GetDayCycleLength() const { return DayCycleLength; }

	FORCEINLINE void SetWorldTimeMultiplier(int32 Multiplier) { WorldTimeMultiplier = Multiplier; }
	FORCEINLINE void SetTotalPlayTime(int64 Time) { TotalPlayTime = Time; }
	FORCEINLINE void SetDayCycleLength(int32 Length) { DayCycleLength = Length ; }
	
	FString CalculateSecondToStr(int64 Time) const;
	void SetWorldTime(FWorldTimeData NewTimeData);


#pragma endregion

#pragma region Subsystem Setup
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

#pragma endregion

#pragma region Public Time Control API
public:
	void StartWorldTime();
	void PauseTime();
	void ResumeTime();
	void ModifyTime(int32 Day, int32 Minute);

#pragma endregion

#pragma region Internal Time Logic
public:
	void UpdateTimeWidget();
protected:

	void UpdateTime();
	void AdvanceSkyTime(int32 Min, int32 Sec);


#pragma endregion

#pragma region Time Settings & State
protected:
	UPROPERTY()
	int32 WorldTimeMultiplier = 1;

	UPROPERTY()
	int32 DayCycleLength = 20;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldTime|State", meta = (AllowPrivateAccess = "true"))
	FWorldTimeData CurrentTimeData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldTime|State", meta = (AllowPrivateAccess = "true"))
	int64 TotalPlayTime = 0; //Seconds

#pragma endregion

#pragma region Timer & References

	UPROPERTY(Transient)
	AEnvironmentManager* EnvironmentManager;

	USeasonManager* SeasonManager;


	FTimerHandle TimeUpdateHandle;

#pragma endregion

#pragma region Delegates & Events

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldTime|Event")
	FOnWorldTimeUpdated OnWorldTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "WorldTime|Event")
	FOnMinuteUpdated OnMinuteUpdated;

#pragma endregion
};
