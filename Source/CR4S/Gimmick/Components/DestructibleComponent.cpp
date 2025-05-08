#include "DestructibleComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UDestructibleComponent::UDestructibleComponent()
	: CurrentHealth(0.f)
	  , MaxHealth(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDestructibleComponent::ApplyDamage(const float DamageAmount, const FVector& HitLocation, const FVector& ImpulseDir,
                                         const float ImpulseStrength)
{
	CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageAmount);

	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %.2f"), CurrentHealth);
	
	if (CurrentHealth <= 0)
	{
		if (AActor* Owner = GetOwner())
		{
			Owner->Destroy();
		}
	}
}

void UDestructibleComponent::ApplyRadiusDamage(float BaseDamage, const FVector& HurtOrigin, float DamageRadius,
                                               float ImpulseStrength, bool bFullDamage)
{
}

void UDestructibleComponent::TakeDamage(const FDestructibleDamageData& DestructibleDamageData)
{
	APlayerController* PlayerController = Cast<APlayerController>(DestructibleDamageData.Instigator);
	if (IsValid(PlayerController))
	{
		UKismetSystemLibrary::PrintString(
			GetWorld(), FString::Printf(TEXT("DamageAmount: %.1f"), DestructibleDamageData.DamageAmount));

		ApplyDamage(
			DestructibleDamageData.DamageAmount,
			DestructibleDamageData.HitLocation,
			DestructibleDamageData.ImpulseDir,
			DestructibleDamageData.ImpulseStrength
		);
	}
	else
	{
		UKismetSystemLibrary::PrintString(
			GetWorld(), FString::Printf(
				TEXT("%s is not PlayerController"), *DestructibleDamageData.Instigator->GetName()));
	}
}
