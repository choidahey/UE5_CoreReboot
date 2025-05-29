#pragma once

#include "GameFramework/Actor.h"
#include "Game/System/SeasonType.h"
#include "EnvironmentManager.generated.h"

UCLASS()
class CR4S_API AEnvironmentManager : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintImplementableEvent, Category = "Sky")
	void SetSkyTime(float Time);
	UFUNCTION(BlueprintImplementableEvent, Category = "Weather")
	void SetWeather(UDataAsset* WeatherPreset, float TransitionTime);
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeatherByName(const FString& WeatherName, float TransitionTime = 15.0f);
	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeatherBySeason(ESeasonType Season, float TransitionTime = 15.0f);

protected:
	AEnvironmentManager();
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;

	//TObjectPtr for Default Weathers, TSoftObjectPtr for Regional Weathers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> BountifulSeasonPreset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> FrostSeasonPreset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> RainySeasonPreset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultSeason")
	TObjectPtr<UDataAsset> DrySeasonPreset;

#pragma region Spawn Zone Volume Management
protected:
	UPROPERTY(EditAnywhere, Category = "Spawn Zone")
	int32 GridSize = 5000;

#pragma endregion
private:

};
