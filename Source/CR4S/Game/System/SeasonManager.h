#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Game/System/SeasonType.h"
#include "SeasonManager.generated.h"

class AEnvironmentManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, float, Ratio);
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

	FORCEINLINE void SetSeasonLength(int32 Length) { SeasonLength = Length; }

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

	float GetTargetDayRatioForSeason(ESeasonType Season) const;

	ESeasonType CurrentSeason = ESeasonType::BountifulSeason;

	int32 SeasonLength = 30;
	int32 CurrentSeasonDay = 1;
	float CurrentDayNightRatio = 0.5f;
	float StartDayNightRatio = 0.5f;
	float TargetDayNightRatio = 0.3f;

#pragma endregion

#pragma region  Delegates & Events

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldTime|Event")
	FOnDayChanged OnDayChanged;
	UPROPERTY(BlueprintAssignable, Category = "WorldTime|Event")
	FOnSeasonChanged OnSeasonChanged;
#pragma endregion
};
