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

	BindToTimeSystemDelegates();
}

void UEnvironmentalStatusComponent::BindToTimeSystemDelegates()
{
	UWorldTimeManager* TimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
	if (TimeManager)
	{
		DayCycleLength = TimeManager->GetDayCycleLength();
		TimeManager->OnMinuteUpdated.AddDynamic(this, &UEnvironmentalStatusComponent::UpdateTemperatureByMinute);
	}
	
	USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
	if(SeasonManager)
	{
		SeasonManager->OnDayChanged.AddDynamic(this, &UEnvironmentalStatusComponent::UpdateDayNightRatio);
		SeasonManager->OnSeasonChanged.AddDynamic(this, &UEnvironmentalStatusComponent::HandleSeasonChanged);
		CurrentSeasonBaseTemperature = GetBaseTemperatureBySeason(SeasonManager->GetCurrentSeason());
		CurrentTemperature = CurrentSeasonBaseTemperature;
		TargetTemperature = CurrentTemperature;
	}
}

void UEnvironmentalStatusComponent::ModifyTemperature(float Delta, float Speed)
{
	TargetTemperature = TargetTemperature + Delta;
	TemperatureChangeSpeed = Speed;

	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UEnvironmentalStatusComponent::ModifyHumidity(float Delta, float Speed)
{
	CurrentHumidity = FMath::Clamp(CurrentHumidity + Delta, MinHumidity, MaxHumidity);

	if (IsPlayer())
	{
		OnHumidityChanged.Broadcast(CurrentHumidity);
	}
}

// 매 분마다 불리는 함수
void UEnvironmentalStatusComponent::UpdateTemperatureByMinute(int32 Minute)
{

}

void UEnvironmentalStatusComponent::UpdateDayNightRatio(float Ratio)
{
	CurrentDayNightRatio = Ratio;
}

void UEnvironmentalStatusComponent::HandleSeasonChanged(ESeasonType Season)
{
	UE_LOG(LogTemp, Log, TEXT("HandleSeasonChanged Called. Season: %d"), static_cast<uint8>(Season));

	float OldBaseTemperature = CurrentSeasonBaseTemperature;
	CurrentSeasonBaseTemperature = GetBaseTemperatureBySeason(Season);

	UE_LOG(LogTemp, Log, TEXT("OldBaseTemperature: %.2f, NewBaseTemperature: %.2f"),
		OldBaseTemperature, CurrentSeasonBaseTemperature);

	float TemperatureDelta = CurrentSeasonBaseTemperature - OldBaseTemperature;
	TargetTemperature = TargetTemperature + TemperatureDelta;

	UE_LOG(LogTemp, Log, TEXT("TargetTemperature Adjusted: %.2f (Delta: %.2f)"), TargetTemperature, TemperatureDelta);

	PrimaryComponentTick.SetTickFunctionEnable(true);
	UE_LOG(LogTemp, Log, TEXT("Tick Enabled"));
}



void UEnvironmentalStatusComponent::SetMaxTemperature(float Max)
{
	// TODO: 구현 예정
}

void UEnvironmentalStatusComponent::SetMinTemperature(float Min)
{
	// TODO: 구현 예정
}

void UEnvironmentalStatusComponent::SetMaxHumidity(float Max)
{
	// TODO: 구현 예정
}

bool UEnvironmentalStatusComponent::IsPlayer() const
{
	AActor* Owner = GetOwner();
	return Owner && (Owner->IsA(APlayerCharacter::StaticClass()) || Owner->IsA(AModularRobot::StaticClass()));
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

	if (IsPlayer())
	{
		OnTemperatureChanged.Broadcast(CurrentTemperature);
	}

	if (CurrentTemperature == TargetTemperature)
	{
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}

}



void UEnvironmentalStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTemperature(DeltaTime, TemperatureChangeSpeed);

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