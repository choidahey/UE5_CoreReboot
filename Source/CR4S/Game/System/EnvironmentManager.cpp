#include "Game/System/EnvironmentManager.h"
#include "Engine/DataAsset.h"
#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonType.h"
#include "Game/System/SpawnZoneManager.h"
#include "Game/System/SeasonManager.h"
#include "Game/System/EnvironmentalModifierVolume.h"

AEnvironmentManager::AEnvironmentManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnvironmentManager::BeginPlay()
{
    // Load Weather if there is Save

    //For NewGame, Season Starts with Bountiful Season

    TimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
    if (TimeManager)
    {
        TimeManager->SetWorldTimeMultiplier(WorldTimeMultiplier);
        TimeManager->StartWorldTime();
    }

    Super::BeginPlay();
}

void AEnvironmentManager::SetDayNightRatio(float Ratio)
{
    Ratio = FMath::Clamp(Ratio, 0.0f, 1.0f);

    int32 DayCycle = TimeManager->GetDayCycleLength();
    int32 Day = FMath::RoundToInt(DayCycle * Ratio);
    int32 Night = DayCycle - Day;

    UE_LOG(LogTemp, Log, TEXT("SetDayNightRatio: Ratio = %.2f | DayCycle = %d | Day = %d | Night = %d"),
        Ratio, DayCycle, Day, Night);

    ApplyDayNightRatio(Day, Night);
}

void AEnvironmentManager::SetWeatherByName(const FString& WeatherName, float TransitionTime)
{
    FString AssetPath = FString::Printf(TEXT("/Game/UltraDynamicSky/Blueprints/Weather_Effects/Weather_Presets/%s.%s"), *WeatherName, *WeatherName);

    UObject* LoadedObject = StaticLoadObject(UDataAsset::StaticClass(), nullptr, *AssetPath);

    if (UDataAsset* WeatherAsset = Cast<UDataAsset>(LoadedObject))
    {
        UE_LOG(LogTemp, Log, TEXT("Weather preset loaded: %s"), *WeatherName);
        ApplyWeather(WeatherAsset, TransitionTime);
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
        ApplyWeather(SelectedPreset, TransitionTime);
        UE_LOG(LogTemp, Log, TEXT("SetWeatherBySeason: Weather set for season %s"), *UEnum::GetValueAsString(Season));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SetWeatherBySeason: Preset is not assigned for season %s"), *UEnum::GetValueAsString(Season));
    }
}


float AEnvironmentManager::GetBaseTemperatureBySeason() const
{
    USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
    if (!SeasonManager) return 20.0f;

    switch (SeasonManager->GetCurrentSeason())
    {
    case ESeasonType::BountifulSeason: return 15.0f;
    case ESeasonType::FrostSeason: return -30.0f;
    case ESeasonType::RainySeason: return 10.0f;
    case ESeasonType::DrySeason: return 40.0f;

    default: return 20.0f;
    }
}

void AEnvironmentManager::SpawnEnvironmentalModifierVolume(
    const FVector& Location,
    float Radius,
    float Height,
    float Duration,
    float TemperatureDelta,
    float HumidityDelta,
    float Speed)
{

}

void AEnvironmentManager::SetWorldTimeMultiplier_Implementation(int32 Multiplier)
{
    TimeManager->SetWorldTimeMultiplier(Multiplier);
}