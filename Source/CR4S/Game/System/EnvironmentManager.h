#pragma once

#include "GameFramework/Actor.h"
#include "Game/System/SeasonType.h"
#include "EnvironmentManager.generated.h"

class UWorldTimeManager;

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

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Weather")
	void ApplyWeather(UDataAsset* WeatherPreset, float TransitionTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "Time")
	void SetDawnDuskTime(float DawnTime, float DuskTime);


#pragma endregion

public:
	FORCEINLINE int32 GetGridSize() { return GridSize; }

	void SpawnEnvironmentalModifierVolume(
		const FVector& Location,
		float Radius,
		float Height,
		float Duration,
		float TemperatureDelta,
		float HumidityDelta,
		float Speed);

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
