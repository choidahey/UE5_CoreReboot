// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusComponent.h"
#include "Character/Data/ModularRobotStatus.h"
#include "ModularRobotStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnergyChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStunChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeightChangedDelegate, float);

class AModularRobot;
class UModularRobotStatusAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UModularRobotStatusComponent : public UBaseStatusComponent
{
	GENERATED_BODY()

public:
	UModularRobotStatusComponent();
	
#pragma region Get
	FORCEINLINE float GetMaxEnergy() const { return RobotStatus.MaxEnergy; }
	FORCEINLINE float GetCurrentEnergy() const { return RobotStatus.Energy; }
	FORCEINLINE float GetEnergyConsumptionRate() const { return RobotStatus.EnergyConsumptionRate; }

	FORCEINLINE float GetMaxStun() const { return RobotStatus.MaxStun; }
	FORCEINLINE float GetCurrentStun() const { return RobotStatus.Stun; }
	FORCEINLINE float GetStunResistance() const { return RobotStatus.StunResistance; }

	FORCEINLINE float GetArmorMultiplier() const { return RobotStatus.ArmorMultiplier; }
	FORCEINLINE float GetAttackPowerMultiplier() const { return RobotStatus.AttackPowerMultiplier; }

	FORCEINLINE float GetMaxWeight() const { return RobotStatus.MaxWeight; }
	FORCEINLINE float GetCurrentWeight() const { return RobotStatus.Weight; }
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
#pragma endregion
	
#pragma region Override
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

	
#pragma region DataAsset
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="StatusData")
	TObjectPtr<UModularRobotStatusAsset> StatusData; 
#pragma endregion

#pragma region Owner
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Owner")
	TObjectPtr<AModularRobot> OwningCharacter;
#pragma endregion
	
#pragma region Status
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FModularRobotStats RobotStatus;
#pragma endregion

#pragma region Delegate
public:
	FOnEnergyChangedDelegate OnEnergyChanged;
	FOnStunChangedDelegate OnStunChanged;
	FOnWeightChangedDelegate OnWeightChanged;
#pragma endregion
};
