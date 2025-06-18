#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthDecayComponent.generated.h"


class UWorldTimeManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UHealthDecayComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UHealthDecayComponent();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region HealthDecay

public:
	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	void StartHealthDecay();
	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	void StopHealthDecay();

	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	void ResetHealthDecay();

	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	FORCEINLINE void SetMaxHealth(const float NewMaxHealth) { MaxHealth = NewMaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	FORCEINLINE void SetCurrentHealth(const float NewCurrentHealth) { CurrentHealth = NewCurrentHealth; }

private:
	UFUNCTION()
	void HandleWorldTimeUpdated(const int64 NewPlayTime);

	UPROPERTY(EditDefaultsOnly, Category = "HealthDecay")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, Category = "HealthDecay")
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, Category = "HealthDecay", meta=(ClampMin="0.0", ClampMax="1.0"))
	float TargetHealthRatio;
	UPROPERTY(EditDefaultsOnly, Category = "HealthDecay", meta=(ClampMin="1.0"))
	float DecayDuration;

	int64 PreviousPlayTime = -1;

	float MinHealth;
	float HealthDecayRate;

	UPROPERTY()
	TObjectPtr<UWorldTimeManager> WorldTimeManager;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateHealthDecay, const float, CurrentHealth);

	UPROPERTY(BlueprintAssignable, Category = "HealthDecay")
	FOnUpdateHealthDecay OnUpdateHealthDecay;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndHealthDecay);

	UPROPERTY(BlueprintAssignable, Category = "HealthDecay")
	FOnEndHealthDecay OnEndHealthDecay;

#pragma endregion
};
