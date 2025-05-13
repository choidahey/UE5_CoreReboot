#include "DestructibleComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UDestructibleComponent::UDestructibleComponent()
	: MaxHealth(100.f)
	  , CurrentHealth(100.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDestructibleComponent::InitComponent(const float InMaxHealth)
{
	MaxHealth = InMaxHealth;
	CurrentHealth = InMaxHealth;
}

void UDestructibleComponent::TakeDamage(AController* DamageCauserController, float DamageAmount)
{
	APlayerController* PlayerController = Cast<APlayerController>(DamageCauserController);
	if (IsValid(PlayerController))
	{
		CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.f);

		if (OnTakeDamage.IsBound())
		{
			OnTakeDamage.ExecuteIfBound(DamageAmount, CurrentHealth);
		}
		
		if (CurrentHealth <= 0.f)
		{
			OnDestroy.ExecuteIfBound();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is not PlayerController"), *DamageCauserController->GetName());
	}
}
