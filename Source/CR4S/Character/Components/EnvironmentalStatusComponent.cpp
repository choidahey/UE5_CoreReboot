#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Game/System/EnvironmentManager.h"
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
	CurrentTemperature = EnvironmentManager->GetBaseTemperatureBySeason();
	TargetTemperature = CurrentTemperature;
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
	const float DampingFactor = 0.1f; 

	float Diff = TargetTemperature - CurrentTemperature;
	float Direction = FMath::Sign(Diff);
	float DeltaTemp = Direction * Speed * FMath::Exp(-FMath::Abs(Diff) * DampingFactor);

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