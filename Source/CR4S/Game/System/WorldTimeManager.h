#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WorldTimeManager.generated.h"

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
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void PauseTime();
	void ResumeTime();

	void ModifyTime(int32 Day, int32 Minute);

protected:

private:
	UFUNCTION()
	void UpdateTime();

	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void SetWorldTimeData();

	UPROPERTY()
	FTimerHandle TimeUpdateHandle;

	UPROPERTY()
	int32 DayLength = 10; // Minute Length in a Day
	UPROPERTY()
	int32 SeasonLength = 30; // Number of Days in a Season

	FWorldTimeData CurrentTimeData;
};
