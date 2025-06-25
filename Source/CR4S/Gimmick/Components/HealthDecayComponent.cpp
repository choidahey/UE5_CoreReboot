#include "HealthDecayComponent.h"

#include "Game/System/WorldTimeManager.h"

UHealthDecayComponent::UHealthDecayComponent()
	: bIsInit(false)
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

	if (!bIsInit)
	{
		ResetHealthDecay();
		bIsInit = true;
	}
}

void UHealthDecayComponent::StartHealthDecay()
{
	if (IsValid(WorldTimeManager) && !HealthDecayData.bIsActive)
	{
		WorldTimeManager->OnWorldTimeUpdated
		                .AddUniqueDynamic(this, &ThisClass::HandleWorldTimeUpdated);

		HealthDecayData.bIsActive = true;
	}
}

void UHealthDecayComponent::StopHealthDecay()
{
	if (IsValid(WorldTimeManager) &&
		WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(this, &ThisClass::HandleWorldTimeUpdated) &&
		HealthDecayData.bIsActive)
	{
		WorldTimeManager->OnWorldTimeUpdated
		                .RemoveDynamic(this, &ThisClass::HandleWorldTimeUpdated);

		HealthDecayData.bIsActive = false;
	}
}

void UHealthDecayComponent::ResetHealthDecay()
{
	HealthDecayData.ResetHealthDecay();
}

void UHealthDecayComponent::HandleWorldTimeUpdated(const int64 NewPlayTime)
{
	if (HealthDecayData.PreviousPlayTime < 0)
	{
		HealthDecayData.PreviousPlayTime = NewPlayTime;
		return;
	}

	const int64 DeltaInt = NewPlayTime - HealthDecayData.PreviousPlayTime;
	HealthDecayData.PreviousPlayTime = NewPlayTime;

	if (DeltaInt <= 0) return;

	const float DeltaSeconds = static_cast<float>(DeltaInt);

	if (HealthDecayData.CurrentHealth > HealthDecayData.MinHealth)
	{
		HealthDecayData.CurrentHealth -= HealthDecayData.HealthDecayRate * DeltaSeconds;
		HealthDecayData.CurrentHealth = FMath::Max(HealthDecayData.CurrentHealth, HealthDecayData.MinHealth);

		if (OnUpdateHealthDecay.IsBound())
		{
			OnUpdateHealthDecay.Broadcast(HealthDecayData.CurrentHealth);
		}

		if (HealthDecayData.CurrentHealth <= HealthDecayData.MinHealth)
		{
			if (OnEndHealthDecay.IsBound())
			{
				OnEndHealthDecay.Broadcast();
			}

			StopHealthDecay();
		}
	}
	else
	{
		if (OnEndHealthDecay.IsBound())
		{
			OnEndHealthDecay.Broadcast();
		}

		StopHealthDecay();
	}
}

void UHealthDecayComponent::LoadHealthDecayData(const FHealthDecayData& NewHealthDecayData)
{
	HealthDecayData = NewHealthDecayData;

	if (HealthDecayData.bIsActive)
	{
		bIsInit = true;
		HealthDecayData.PreviousPlayTime = -1;
		HealthDecayData.bIsActive = false;
		StartHealthDecay();
	}
}
