#include "Game/System/EnvironmentManager.h"
#include "Engine/DataAsset.h"
#include "Game/System/SeasonType.h"

AEnvironmentManager::AEnvironmentManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnvironmentManager::BeginPlay()
{
    // Load Weather if there is Save

    //For NewGame, Season Starts with Bountiful Season
}

void AEnvironmentManager::SetWeatherByName(const FString& WeatherName, float TransitionTime)
{
    FString AssetPath = FString::Printf(TEXT("/Game/UltraDynamicSky/Blueprints/Weather_Effects/Weather_Presets/%s.%s"), *WeatherName, *WeatherName);

    UObject* LoadedObject = StaticLoadObject(UDataAsset::StaticClass(), nullptr, *AssetPath);

    if (UDataAsset* WeatherAsset = Cast<UDataAsset>(LoadedObject))
    {
        UE_LOG(LogTemp, Log, TEXT("Weather preset loaded: %s"), *WeatherName);
        SetWeather(WeatherAsset, TransitionTime);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load weather preset: %s"), *AssetPath);
    }
}

void AEnvironmentManager::SetWeatherBySeason(ESeasonType Season, float TransitionTime)
{
    UDataAsset* SelectedPreset = nullptr;

    switch (Season)
    {
    case ESeasonType::BountifulSeason:
        SelectedPreset = BountifulSeasonPreset;
        break;
    case ESeasonType::FrostSeason:
        SelectedPreset = FrostSeasonPreset;
        break;
    case ESeasonType::RainySeason:
        SelectedPreset = RainySeasonPreset;
        break;
    case ESeasonType::DrySeason:
        SelectedPreset = DrySeasonPreset;
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("SetWeatherBySeason: Unknown season type"));
        return;
    }

    if (SelectedPreset)
    {
        SetWeather(SelectedPreset, TransitionTime);
        UE_LOG(LogTemp, Log, TEXT("SetWeatherBySeason: Weather set for season %s"), *UEnum::GetValueAsString(Season));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SetWeatherBySeason: Preset is not assigned for season %s"), *UEnum::GetValueAsString(Season));
    }
}
