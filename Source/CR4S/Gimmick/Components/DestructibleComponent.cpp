#include "DestructibleComponent.h"

#include "Character/CharacterController.h"

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

void UDestructibleComponent::TakeDamage(AController* DamageCauserController, const float DamageAmount)
{
	if (!bCanTakeDamage || IsDestructed())
	{
		return;
	}

	bCanTakeDamage = false;

	LastDamageCauserController = DamageCauserController;

	CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.f);

	if (OnTakeDamage.IsBound())
	{
		OnTakeDamage.ExecuteIfBound(DamageAmount, CurrentHealth);
	}

	if (IsDestructed())
	{
		OnDestroy.ExecuteIfBound();
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
