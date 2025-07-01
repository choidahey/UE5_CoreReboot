#pragma once

#include "GameFramework/Actor.h"
#include "Game/System/SeasonType.h"
#include "EnvironmentManager.generated.h"

class UWorldTimeManager;
class AEnvironmentalModifierVolume;

UCLASS()
class CR4S_API AEnvironmentManager : public AActor
{
	GENERATED_BODY()

#pragma region UltraDynamic Sky and Weather Controls
public:	
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetDayNightByTime(float DawnTime, float DuskTime);
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeatherByName(const FString& WeatherName, float TransitionTime = 60.0f);
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeatherBySeason(ESeasonType Season, float TransitionTime = 60.0f);
	UFUNCTION(BlueprintImplementableEvent, Category = "Time")
	void SetSkyTime(float Time);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Time")
	float GetCurrentTimeOfDay() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Time")
	float GetCurrentDawnTime() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Time")
	float GetCurrentDuskTime() const;


	UFUNCTION(BlueprintCallable, Category = "Time")
	void AdvanceTimeOfDay(float TimeOfDay);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Time")
	bool IsSleepTime() const;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Weather")
	void ApplyWeather(UDataAsset* WeatherPreset, float TransitionTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "Time")
	void SetDawnDuskTime(float DawnTime, float DuskTime);


#pragma endregion

public:
	FORCEINLINE int32 GetGridSize() { return GridSize; }

	UFUNCTION(BlueprintCallable, Category = "Environment")
	AEnvironmentalModifierVolume* SpawnEnvModVol(
		const FVector& Location,
		float Radius = 1000.0f, // Capsule Raidius
		float Height = 1000.0f,	// Capsule Half Height
		float TempDelta = 0.0f,
		float HumidDelta = 0.0f,
		float ChangeSpeed = 2.0f, 	// 2.0f is Suggested for Monster Skills, 0.5f for Gimmicks
		float Duration = -1.0f 	// Negative Value for Persistent Duration, Setting Positive Value Defines How Long the Volume Lasts 

	);

	bool IsEnvironmentReady();
	bool bHasSetWeather = false;

protected:
	AEnvironmentManager();
	virtual void BeginPlay() override;

#pragma region Weather Management

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> BountifulSeasonPreset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> FrostSeasonPreset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> RainySeasonPreset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> DrySeasonPreset;
#pragma endregion

#pragma region WorldTime Managemet
protected:

	// The main purpose for setting time multiplier here is for testing
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "World Time")
	void SetWorldTimeMultiplier(int32 Multiplier);
	virtual void SetWorldTimeMultiplier_Implementation(int32 Multiplier);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Time")
	int32 WorldTimeMultiplier = 1;

	UWorldTimeManager* TimeManager;

#pragma endregion

#pragma region Spawn Zone Volume Management
protected:
	UPROPERTY(EditAnywhere, Category = "Spawn Zone")
	int32 GridSize = 5000;

#pragma endregion
private:

};
