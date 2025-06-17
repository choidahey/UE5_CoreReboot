#include "HealthDecayComponent.h"

#include "Game/System/WorldTimeManager.h"

UHealthDecayComponent::UHealthDecayComponent()
	: MaxHealth(7000.f),
	  CurrentHealth(7000.f),
	  TargetHealthRatio(0.2f),
	  DecayDuration(2400.f),
	  MinHealth(0),
	  HealthDecayRate(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthDecayComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (IsValid(World))
	{
		WorldTimeManager = World->GetSubsystem<UWorldTimeManager>();
	}

	ResetHealthDecay();
}

void UHealthDecayComponent::StartHealthDecay()
{
	if (IsValid(WorldTimeManager))
	{
		WorldTimeManager->OnWorldTimeUpdated
		                .AddUniqueDynamic(this, &ThisClass::HandleWorldTimeUpdated);
	}
}

void UHealthDecayComponent::StopHealthDecay()
{
	if (IsValid(WorldTimeManager) &&
		WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(this, &ThisClass::HandleWorldTimeUpdated))
	{
		WorldTimeManager->OnWorldTimeUpdated
		                .RemoveDynamic(this, &ThisClass::HandleWorldTimeUpdated);
	}

	if (OnEndHealthDecay.IsBound())
	{
		OnEndHealthDecay.Broadcast();
	}
}

void UHealthDecayComponent::ResetHealthDecay()
{
	PreviousPlayTime = -1;
	CurrentHealth = MaxHealth;
	MinHealth = MaxHealth * TargetHealthRatio;
	HealthDecayRate = (MaxHealth - MinHealth) / DecayDuration;
}

void UHealthDecayComponent::HandleWorldTimeUpdated(const int64 NewPlayTime)
{
	if (PreviousPlayTime < 0)
	{
		PreviousPlayTime = NewPlayTime;
		return;
	}

	const int64 DeltaInt = NewPlayTime - PreviousPlayTime;
	PreviousPlayTime = NewPlayTime;

	if (DeltaInt <= 0) return;

	const float DeltaSeconds = static_cast<float>(DeltaInt);

	if (CurrentHealth > MinHealth)
	{
		CurrentHealth -= HealthDecayRate * DeltaSeconds;
		CurrentHealth = FMath::Max(CurrentHealth, MinHealth);

		if (OnUpdateHealthDecay.IsBound())
		{
			OnUpdateHealthDecay.Broadcast(CurrentHealth);
		}

		if (CurrentHealth <= MinHealth)
		{
			StopHealthDecay();
		}
	}
	else
	{
		StopHealthDecay();
	}
}
