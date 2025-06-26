#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DestructibleComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UDestructibleComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UDestructibleComponent();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Initialize

public:
	void InitComponent(const float InMaxHealth);

#pragma endregion

#pragma region Health

public:
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	FORCEINLINE bool IsDestructed() const { return CurrentHealth <= 0.0f; }

	FORCEINLINE void SetMaxHealth(const float InMaxHealth)
	{
		MaxHealth = InMaxHealth;
	}

	FORCEINLINE void SetCurrentHealth(const float InCurrentHealth)
	{
		CurrentHealth = InCurrentHealth;
	}

	FORCEINLINE void Repair(const float Amount)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	}

	FORCEINLINE void SetCanRepair(const bool bNewCanRepair) { bCanRepair = bNewCanRepair; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	bool bCanRepair;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth;

#pragma endregion

#pragma region Hit

public:
	UFUNCTION(BlueprintCallable, Category = "DestructibleComponent|Hit")
	void TakeDamage(AActor* DamageCauser, const float DamageAmount);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Hit", meta = (ClampMin = "0.0"))
	float HitRecoveryTime;

	UPROPERTY(VisibleAnywhere, Category = "Hit")
	bool bCanTakeDamage;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnTakeDamage, AActor*, DamageCauser, float, DamageAmount, float,
	                                     CurrentHealth);

	FOnTakeDamage OnTakeDamage;
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDestry, AActor*, DamageCauser);

	FOnDestry OnDestroy;

#pragma endregion
};
