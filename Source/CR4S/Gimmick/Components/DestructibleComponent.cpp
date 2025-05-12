#include "DestructibleComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UDestructibleComponent::UDestructibleComponent()
	: MaxHealth(100.f)
	  , CurrentHealth(100.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDestructibleComponent::TakeDamage(AController* DamageCauserController, float DamageAmount)
{
	APlayerController* PlayerController = Cast<APlayerController>(DamageCauserController);
	if (IsValid(PlayerController))
	{
		UKismetSystemLibrary::PrintString(
			GetWorld(), FString::Printf(TEXT("DamageAmount: %.1f"), DamageAmount));

		CurrentHealth = FMath::Max(CurrentHealth - DamageAmount, 0.f);

		UKismetSystemLibrary::PrintString(
			GetWorld(), FString::Printf(TEXT("CurrentHealth: %.1f"), CurrentHealth));

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
