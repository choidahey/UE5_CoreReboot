#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Game/System/EnvironmentManager.h"
#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonManager.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Character/Characters/ModularRobot.h"
#include "Kismet/GameplayStatics.h"

UEnvironmentalStatusComponent::UEnvironmentalStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UEnvironmentalStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	EnvironmentManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass()));

	InitializeTimeSystemBindings();
}

void UEnvironmentalStatusComponent::InitializeTimeSystemBindings()
{
	UWorldTimeManager* TimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
	if (TimeManager)
	{
		DayCycleLength = TimeManager->GetDayCycleLength();
		TimeManager->OnMinuteUpdated.AddDynamic(this, &UEnvironmentalStatusComponent::UpdateTemperatureByMinute);
	}

	USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
	if (SeasonManager)
	{
		ESeasonType CurrentSeason = SeasonManager->GetCurrentSeason();
		CurrentSeasonBaseTemperature = GetBaseTemperatureBySeason(CurrentSeason);
		CurrentSeasonBaseHumidity = GetBaseHumidityBySeason(CurrentSeason);

		CurrentTemperature = CurrentSeasonBaseTemperature;
		CurrentHumidity = CurrentSeasonBaseHumidity;

		TargetTemperature = CurrentTemperature;
		TargetHumidity = CurrentHumidity;

		CurrentDawnTime = SeasonManager->GetCurrentDawnTime();
		CurrentDuskTime = SeasonManager->GetCurrentDuskTime();

		UpdateDayLengthChanged(CurrentDawnTime, CurrentDuskTime);
		UpdateTemperatureByMinute(0, DayCycleLength);

		SeasonManager->OnDayChanged.AddDynamic(this, &UEnvironmentalStatusComponent::UpdateDayLengthChanged);
		SeasonManager->OnSeasonChanged.AddDynamic(this, &UEnvironmentalStatusComponent::HandleSeasonChanged);
	}
}

void UEnvironmentalStatusComponent::UpdateTemperatureByMinute(int32 Minute, int32 DayCycle)
{
	float TimeProgress = float(Minute) / float(DayCycle); // 0.0 ~ 1.0

	float NormalizedDayMid = DayMid / 2400.0f;
	float NormalizedNightMid = NightMid / 2400.0f;

	float PhaseProgress = (TimeProgress - NormalizedDayMid) / (NormalizedNightMid - NormalizedDayMid);
	float Angle = PhaseProgress * PI;

	float SineValue = FMath::Cos(Angle);

	const float DailyTempAmplitude = 10.0f;
	TemperatureOffset = SineValue * DailyTempAmplitude * 0.5f;

	TargetTemperature = CurrentSeasonBaseTemperature + TemperatureOffset + ExternalTemperatureDelta;

	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UEnvironmentalStatusComponent::UpdateDayLengthChanged(float DawnTime, float DuskTime)
{
	DayMid = (DawnTime + DuskTime) / 2.0f;
	NightMid = (DuskTime + DawnTime + 2400.0f) / 2.0f;
}

void UEnvironmentalStatusComponent::HandleSeasonChanged(ESeasonType Season)
{
	float OldBaseTemperature = CurrentSeasonBaseTemperature;
	float OldBaseHumidity = CurrentSeasonBaseHumidity;

	CurrentSeasonBaseTemperature = GetBaseTemperatureBySeason(Season);
	CurrentSeasonBaseHumidity = GetBaseHumidityBySeason(Season);

	float TemperatureDelta = CurrentSeasonBaseTemperature - OldBaseTemperature;
	float HumidityDelta = CurrentSeasonBaseHumidity - OldBaseHumidity;

	TargetTemperature = TargetTemperature + TemperatureDelta;
	TargetHumidity = TargetHumidity + HumidityDelta;

	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UEnvironmentalStatusComponent::ModifyTemperature(float Delta, float Speed)
{
	ExternalTemperatureDelta += Delta;
	TemperatureChangeSpeed = Speed;
	TargetTemperature = CurrentSeasonBaseTemperature + TemperatureOffset + ExternalTemperatureDelta;

	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UEnvironmentalStatusComponent::ModifyHumidity(float Delta, float Speed)
{
	ExternalHumidityDelta += Delta;
	HumidityChangeSpeed = Speed;
	TargetHumidity = CurrentSeasonBaseHumidity + ExternalHumidityDelta;

	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UEnvironmentalStatusComponent::UpdateTemperature(float DeltaTime, float Speed)
{
	const float AccelerationFactor = 0.5f;

	float Diff = TargetTemperature - CurrentTemperature;
	float Direction = FMath::Sign(Diff);
	float DeltaTemp = Direction * Speed * (FMath::Abs(Diff) * AccelerationFactor);

	CurrentTemperature += DeltaTemp * DeltaTime;

	if (FMath::Abs(Diff) < 0.1f)
	{
		CurrentTemperature = TargetTemperature;
	}

	OnTemperatureChanged.Broadcast(CurrentTemperature);

	if (ShouldDisableTick())
	{
		CurrentTemperature = TargetTemperature;
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
}

void UEnvironmentalStatusComponent::UpdateHumidity(float DeltaTime, float Speed)
{
	const float AccelerationFactor = 0.5f;

	float Diff = TargetHumidity - CurrentHumidity;
	float Direction = FMath::Sign(Diff);
	float DeltaHumidity = Direction * Speed * (FMath::Abs(Diff) * AccelerationFactor);

	CurrentHumidity += DeltaHumidity * DeltaTime;

	if (FMath::Abs(Diff) < 0.1f)
	{
		CurrentHumidity = TargetHumidity;
	}

	OnHumidityChanged.Broadcast(CurrentHumidity);

	if (ShouldDisableTick())
	{
		CurrentHumidity = TargetHumidity;
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
}

bool UEnvironmentalStatusComponent::ShouldDisableTick()
{
	return FMath::Abs(CurrentHumidity - TargetHumidity) < 0.01f &&
		FMath::Abs(CurrentTemperature - TargetTemperature) < 0.01f;
}

void UEnvironmentalStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTemperature(DeltaTime, TemperatureChangeSpeed);
	UpdateHumidity(DeltaTime, HumidityChangeSpeed);
}

float UEnvironmentalStatusComponent::GetBaseTemperatureBySeason(ESeasonType Season) const
{
	switch (Season)
	{
	case ESeasonType::BountifulSeason: return 15.0f;
	case ESeasonType::FrostSeason: return -30.0f;
	case ESeasonType::RainySeason: return 10.0f;
	case ESeasonType::DrySeason: return 40.0f;
	default: return 20.0f;
	}
}

float UEnvironmentalStatusComponent::GetBaseHumidityBySeason(ESeasonType Season) const
{
	switch (Season)
	{
	case ESeasonType::BountifulSeason: return 70.0f;
	case ESeasonType::FrostSeason: return 30.0f;
	case ESeasonType::RainySeason: return 130.0f;
	case ESeasonType::DrySeason: return 0.0f;
	default: return 70.0f;
	}
}


void UEnvironmentalStatusComponent::SetMaxTemperature(float Max)
{
	// TODO:
}

void UEnvironmentalStatusComponent::SetMinTemperature(float Min)
{
	// TODO:
}

void UEnvironmentalStatusComponent::SetMaxHumidity(float Max)
{
	// TODO:
}
