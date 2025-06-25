// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusComponent.h"
#include "Character/Data/ModularRobotStatus.h"
#include "ModularRobotStatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnergyChangedDelegate, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStunChangedDelegate, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeightChangedDelegate, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxWeightChangedDelegate, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmLoadChangedDelegate, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxArmLoadChangedDelegate, float, NewValue);

class AModularRobot;
class UModularRobotStatusAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UModularRobotStatusComponent : public UBaseStatusComponent
{
	GENERATED_BODY()

public:
	UModularRobotStatusComponent();

#pragma region CheckWeight
	void CheckTotalWeightCapacity();
	void CheckArmCapacity();

	FORCEINLINE bool IsOverWeighted() const { return bExceedsTotalWeightLimit; }
	FORCEINLINE bool IsArmOverWeighted() const { return bExceedsArmWeightLimit; }
#pragma endregion
	
#pragma region Refresh
	virtual void Refresh() override;
#pragma endregion
	
#pragma region Get & Set
	FORCEINLINE float GetMaxEnergy() const { return RobotStatus.MaxEnergy; }
	FORCEINLINE float GetCurrentEnergy() const { return RobotStatus.Energy; }
	FORCEINLINE float GetEnergyConsumptionRate() const { return RobotStatus.EnergyConsumptionAmount; }

	FORCEINLINE float GetMaxStun() const { return RobotStatus.MaxStun; }
	FORCEINLINE float GetCurrentStun() const { return RobotStatus.Stun; }
	FORCEINLINE float GetStunResistance() const { return RobotStatus.StunResistance; }

	FORCEINLINE float GetArmorMultiplier() const { return RobotStatus.ArmorMultiplier; }
	FORCEINLINE float GetAttackPowerMultiplier() const { return RobotStatus.AttackPowerMultiplier; }

	FORCEINLINE float GetMaxWeight() const { return RobotStatus.MaxWeight; }
	FORCEINLINE float GetCurrentWeight() const { return RobotStatus.Weight; }
	FORCEINLINE float GetRecoilModifier() const { return RobotStatus.RecoilModifier; }

	void SetEnergyConsumptionAmount(const float NewAmount);
	FORCEINLINE void ResetEnergyConsumptionAmount() { RobotStatus.EnergyConsumptionAmount=DefaultRobotStatus.EnergyConsumptionAmount; }
#pragma endregion

#pragma region Add
	void AddMaxEnergy(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddEnergy(const float InAmount);

	void AddMaxStun(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddStun(const float InAmount);
	void AddStunResistance(const float InAmount);

	void AddArmorMultiplier(const float InAmount);
	void AddAttackPowerMultiplier(const float InAmount);

	void AddMaxWeight(const float InAmount);
	void AddWeight(const float InAmount);
	void AddMaxArmMountWeight(const float InAmount);
	void AddCurrentArmMountWeight(const float InAmount);
#pragma endregion

#pragma region Modifier
	void ApplyEnergyEfficiency(const float Modifier);
	void RevertEnergyEfficiency(const float Modifier);

	void ApplyRecoilModifier(const float Modifier);
	void RevertRecoilModifier(const float Modifier);

	void ApplyMeleeDamageModifier(const float Modifier);
	void RevertMeleeDamageModifier(const float Modifier);
#pragma endregion
	
#pragma region Hover
public:
	void StartHover();
	void StopHover();
	void ConsumeResourceForHovering();
#pragma endregion
	
#pragma region Override
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

	
#pragma region Stun
public:
	void RemoveStunDebuff();
#pragma endregion

#pragma region Energy
public:
	FORCEINLINE bool IsRobotActive() const { return bIsRobotActive; }
	void StartConsumeEnergy();
	void StopConsumeEnergy();
	void ConsumeEnergyForInterval();
#pragma endregion

#pragma region Temperature
	virtual void ApplyHeatDebuff() override;
	virtual void RemoveHeatDebuff() override;

	virtual void ApplyColdDebuff() override;
	virtual void RemoveColdDebuff() override;
#pragma endregion

#pragma region Humidity
	virtual void ApplyHighHumidityDebuff() override;
	virtual void RemoveHighHumidityDebuff() override;
#pragma endregion
	
#pragma region DataAsset
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="StatusData")
	TObjectPtr<UModularRobotStatusAsset> StatusData; 
#pragma endregion

#pragma region Cached
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Owner")
	TObjectPtr<AModularRobot> OwningCharacter;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="State")
	uint8 bIsStunned:1 {false};
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="State")
	uint8 bIsRobotActive:1 {true};
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="State")
	uint8 bExceedsTotalWeightLimit:1 {false};
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="State")
	uint8 bExceedsArmWeightLimit:1 {false};
#pragma endregion
	
#pragma region Status
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FModularRobotStats RobotStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FModularRobotStats DefaultRobotStatus;
#pragma endregion

#pragma region Delegate
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Delegate")
	FOnEnergyChangedDelegate OnEnergyChanged;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Delegate")
	FOnStunChangedDelegate OnStunChanged;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Delegate")
	FOnWeightChangedDelegate OnWeightChanged;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Delegate")
	FOnMaxWeightChangedDelegate OnMaxWeightChanged;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Delegate")
	FOnArmLoadChangedDelegate OnArmLoadChanged;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Delegate")
	FOnMaxArmLoadChangedDelegate OnMaxArmLoadChanged;
#pragma endregion

#pragma region Timer
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Timer")
	FTimerHandle StunTimerHandle;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Timer")
	FTimerHandle EnergyTimerHandle;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Timer")
	FTimerHandle HoverTimerHandle;
#pragma endregion
};
