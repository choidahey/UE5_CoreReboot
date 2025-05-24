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

#pragma endregion

#pragma region Initialize

public:
	void InitComponent(const float InMaxHealth);

#pragma endregion

#pragma region Health

public:
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE bool IsDestructed() const { return CurrentHealth <= 0.0f; }

	FORCEINLINE void SetMaxHealth(const float InMaxHealth)
	{
		MaxHealth = InMaxHealth;
		CurrentHealth = MaxHealth;
	}

private:
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth;

#pragma endregion

#pragma region Hit

public:
	UFUNCTION(BlueprintCallable, Category = "DestructibleComponent|Hit")
	void TakeDamage(AActor* DamageCauser, float DamageAmount);

	FORCEINLINE AActor* GetLastDamageCauser() const { return LastDamageCauser; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Hit", meta = (ClampMin = "0.0"))
	float HitRecoveryTime;

	UPROPERTY(VisibleAnywhere, Category = "Hit")
	bool bCanTakeDamage;

	UPROPERTY(VisibleAnywhere, Category = "Hit")
	TObjectPtr<AActor> LastDamageCauser;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnTakeDamage, float, DamageAmount, float, CurrentHealth);
	FOnTakeDamage OnTakeDamage;
	DECLARE_DYNAMIC_DELEGATE(FOnDestry);
	FOnDestry OnDestroy;

#pragma endregion
};
