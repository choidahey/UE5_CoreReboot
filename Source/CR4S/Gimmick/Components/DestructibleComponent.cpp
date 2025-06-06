#include "DestructibleComponent.h"

UDestructibleComponent::UDestructibleComponent()
	: MaxHealth(100.f)
	  , CurrentHealth(100.f)
	  , HitRecoveryTime(0.2f)
	  , bCanTakeDamage(true)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDestructibleComponent::InitComponent(const float InMaxHealth)
{
	MaxHealth = InMaxHealth;
	CurrentHealth = InMaxHealth;
}

void UDestructibleComponent::TakeDamage(AActor* DamageCauser, const float DamageAmount)
{
	if (!bCanTakeDamage || IsDestructed())
	{
		return;
	}

	bCanTakeDamage = false;

	CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.f);

	if (OnTakeDamage.IsBound())
	{
		OnTakeDamage.ExecuteIfBound(DamageCauser, DamageAmount, CurrentHealth);
	}

	if (IsDestructed())
	{
		OnDestroy.ExecuteIfBound(DamageCauser);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[&]()
		{
			if (IsValid(this))
			{
				bCanTakeDamage = true;
			}
		},
		HitRecoveryTime,
		false
	);
}
