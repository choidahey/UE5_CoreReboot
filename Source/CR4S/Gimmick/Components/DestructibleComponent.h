#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DestructibleComponent.generated.h"

DECLARE_DELEGATE(FOnDestry);
DECLARE_DELEGATE_TwoParams(FOnTakeDamage, float, float);

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
	void TakeDamage(AController* DamageCauserController, float DamageAmount);

	FORCEINLINE bool IsDestructed() const { return CurrentHealth <= 0.0f; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Hit", meta = (ClampMin = "0.0"))
	float HitRecoveryTime;
	
	UPROPERTY(VisibleAnywhere, Category = "Hit")
	bool bCanTakeDamage;
	
#pragma endregion

#pragma region Delegate

public:
	FOnTakeDamage OnTakeDamage;
	FOnDestry OnDestroy;

#pragma endregion
};
