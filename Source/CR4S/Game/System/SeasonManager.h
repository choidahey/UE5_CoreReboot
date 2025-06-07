#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Game/System/SeasonType.h"
#include "SeasonManager.generated.h"

class AEnvironmentManager;
class USeasonBossDataAsset;
class ASeasonBossMonster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayChanged, float, DawnTime, float, DuskTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSeasonChanged, ESeasonType, Season);

UCLASS()
class CR4S_API USeasonManager : public UWorldSubsystem
{
	GENERATED_BODY()

#pragma region Subsystem Setup
protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);

	virtual void Deinitialize() override;
	
	void LoadSeason();
#pragma endregion

#pragma region Getters & Setters
public:
	FORCEINLINE ESeasonType GetCurrentSeason() const { return CurrentSeason; }
	FORCEINLINE int32 GetSeasonLength() const { return SeasonLength; }
	FORCEINLINE float GetCurrentDawnTime() const { return CurrentDawnTime; }
	FORCEINLINE float GetCurrentDuskTime() const { return CurrentDuskTime; }
	FORCEINLINE void SetSeasonLength(int32 Length) { SeasonLength = Length; }

	void GetTargetDawnDuskTimeForSeason(ESeasonType Season, float& OutDawnTime, float& OutDuskTime);
	void SetCurrentSeason(ESeasonType NewSeason);


	
	void HandleDayChange();

	void ChangeToNextSeason();

#pragma endregion

#pragma region Internal Season Logic
protected:
	UPROPERTY(Transient)
	AEnvironmentManager* EnvironmentManager;

private:
	void HandleSeasonChange(ESeasonType NewSeason);

	ESeasonType CurrentSeason = ESeasonType::BountifulSeason;

	int32 SeasonLength = 30;
	int32 CurrentSeasonDay = 1;
	float CurrentDawnTime = 600.0f;
	float CurrentDuskTime = 1600.0f;

	float StartDawnTime = 600.0f;
	float TargetDawnTime;

	float StartDuskTime = 1600.0f;
	float TargetDuskTime;

#pragma endregion

#pragma region Events
public:
	TSubclassOf<ASeasonBossMonster> GetSeasonBossClass(ESeasonType Season) const;

	void SpawnSeasonBoss();

protected:


	UPROPERTY()
	USeasonBossDataAsset* SeasonBossData;

	UPROPERTY()
	ASeasonBossMonster* SpawnedBoss;

#pragma endregion

#pragma region Delegates

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldTime|Event")
	FOnDayChanged OnDayChanged;
	UPROPERTY(BlueprintAssignable, Category = "WorldTime|Event")
	FOnSeasonChanged OnSeasonChanged;
#pragma endregion
};
