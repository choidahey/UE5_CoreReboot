#include "DestructibleComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UDestructibleComponent::UDestructibleComponent()
	: MaxHealth(100.f)
	  , CurrentHealth(100.f)
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
	
	const APlayerController* PlayerController = Cast<APlayerController>(DamageCauserController);
	if (IsValid(PlayerController))
	{
		CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.f);

		if (OnTakeDamage.IsBound())
		{
			OnTakeDamage.ExecuteIfBound(DamageAmount, CurrentHealth);
		}

		if (IsDestructed())
		{
			OnDestroy.ExecuteIfBound();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is not PlayerController"), *DamageCauserController->GetName());
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
	0.2f,
	false
	);
}
