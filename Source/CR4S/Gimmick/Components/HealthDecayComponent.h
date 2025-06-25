#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gimmick/Data/HealthDecayData.h"
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

	UFUNCTION(BlueprintPure, Category = "HealthDecay")
	FORCEINLINE FHealthDecayData GetHealthDecayData() const { return HealthDecayData; }

	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	FORCEINLINE void SetMaxHealth(const float NewMaxHealth) { HealthDecayData.MaxHealth = NewMaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	FORCEINLINE void SetCurrentHealth(const float NewCurrentHealth)
	{
		HealthDecayData.CurrentHealth = NewCurrentHealth;
	}

private:
	UFUNCTION()
	void HandleWorldTimeUpdated(const int64 NewPlayTime);

	UPROPERTY(EditDefaultsOnly, Category = "HealthDecay")
	FHealthDecayData HealthDecayData;

	UPROPERTY()
	TObjectPtr<UWorldTimeManager> WorldTimeManager;

	bool bIsInit;

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

#pragma region Load

public:
	UFUNCTION(BlueprintCallable, Category = "HealthDecay")
	void LoadHealthDecayData(const FHealthDecayData& NewHealthDecayData);
	
#pragma endregion 
};
