// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Data/PlayerCharacterStatus.h"
#include "Components/ActorComponent.h"
#include "BaseStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResourceChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColdThresholdChangedDelegate, float, NewThreshold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeatThresholdChangedDelegate, float, NewThreshold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHumidityThresholdChangedDelegate, float, NewThreshold);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UBaseStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseStatusComponent();

#pragma region Refresh
	virtual void Refresh();
#pragma endregion

#pragma region Get  Set
	FORCEINLINE float GetMaxHP() const { return BaseStatus.MaxHealth; }
	FORCEINLINE float GetCurrentHP() const { return BaseStatus.Health; }
	
	FORCEINLINE float GetMaxResource() const { return BaseStatus.MaxResource; }
	FORCEINLINE float GetCurrentResource() const { return BaseStatus.Resource; }
	
	FORCEINLINE float GetAttackPower() const { return BaseStatus.AttackPower; }
	FORCEINLINE float GetArmor() const { return BaseStatus.Armor; }
	
	FORCEINLINE float GetColdThreshold() const { return BaseStatus.ColdThreshold; }
	FORCEINLINE float GetHeatThreshold() const { return BaseStatus.HeatThreshold; }
	FORCEINLINE float GetHumidityThreshold() const { return BaseStatus.HumidityThreshold; }
#pragma endregion
	
#pragma region Set & Reset
	void SetMaxHP(const float NewValue);
	void SetCurrentHP(const float NewValue);
	
	void SetMaxResource(const float NewValue);
	void SetCurrentResource(const float NewValue);
	
	void SetArmor(const float NewValue);
	void SetAttackPower(const float NewValue);
	
	void SetColdThreshold(const float NewValue);
	void SetHeatThreshold(const float NewValue);
	void SetHumidityThreshold(const float NewValue);
	
	void SetResourceConsumptionAmount(const float NewCost);
	FORCEINLINE void ResetResourceConsumptionAmount() { BaseStatus.ResourceConsumptionAmount = DefaultBaseStatus.ResourceConsumptionAmount; }
	
	void SetResourceRegenDelay(const float NewDelay);
	FORCEINLINE void ResetResourceRegenDelay() { BaseStatus.ResourceRegenDelay = DefaultBaseStatus.ResourceRegenDelay; }
#pragma endregion

#pragma region Add
	UFUNCTION(BlueprintCallable)
	void AddMaxHP(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddCurrentHP(const float InAmount);
	
	void AddMaxResource(const float InAmount);
	void AddCurrentResource(const float InAmount);
	
	void AddAttackPower(const float InAmount);
	void AddArmor(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddColdThreshold(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddHeatThreshold(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddHumidityThreshold(const float InAmount);
#pragma endregion

#pragma region Modifier
	void ApplyResourceRegenModifier(const float Modifier);
	void RevertResourceRegenModifier(const float Modifier);

	void ApplyResourceConsumptionModifier(const float Modifier);
	void RevertResourceConsumptionModifier(const float Modifier);
#pragma endregion
	
#pragma region Roll & Dash
	bool HasEnoughResourceForRoll() const;
	void ConsumeResourceForRoll();
#pragma endregion

#pragma region StaminaRegen
	void StartResourceRegen();
	void StopResourceRegen();
	void OnResourceConsumed();
	void RegenResourceForInterval();
#pragma endregion

#pragma region Temperature 
	UFUNCTION()
	void HandleTemperatureChanged(float NewTemperature);

	virtual void ApplyHeatDebuff();
	virtual void RemoveHeatDebuff();
	void ApplyOverHeatDamage();

	virtual void ApplyColdDebuff();
	virtual void RemoveColdDebuff();
#pragma endregion

#pragma region Humidity
	UFUNCTION()
	void HandleHumidityChanged(float NewHumidity);

	virtual void ApplyHighHumidityDebuff();
	virtual void RemoveHighHumidityDebuff();
#pragma endregion
	
#pragma region Override
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion
	
#pragma region Status
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FBaseStats BaseStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FBaseStats DefaultBaseStatus;
#pragma endregion
	
#pragma region Delegate
public:
	FOnHPChangedDelegate OnHPChanged;
	FOnResourceChangedDelegate OnResourceChanged;
	FOnDeathDelegate OnDeathState;
	UPROPERTY(BlueprintAssignable)
	FOnColdThresholdChangedDelegate OnColdThresholdChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHeatThresholdChangedDelegate OnHeatThresholdChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHumidityThresholdChangedDelegate OnHumidityThresholdChanged;
#pragma endregion

#pragma region Cached
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bIsOverHeated:1 {false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bIsFreezing:1 {false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bIsHumidityAffected:1 {false};
#pragma endregion

#pragma region Timer
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FTimerHandle StaminaRegenTimerHandle;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FTimerHandle OverHeatDamageTimerHandle;
#pragma endregion
};

