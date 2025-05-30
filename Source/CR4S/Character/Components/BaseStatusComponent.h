// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/PlayerCharacterStatus.h"
#include "Components/ActorComponent.h"
#include "BaseStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResourceChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UBaseStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseStatusComponent();
#pragma region Get
	FORCEINLINE float GetMaxHP() const { return BaseStatus.MaxHealth; }
	FORCEINLINE float GetCurrentHP() const { return BaseStatus.Health; }
	
	FORCEINLINE float GetMaxResource() const { return BaseStatus.MaxResource; }
	FORCEINLINE float GetCurrentResource() const { return BaseStatus.Resource; }
	FORCEINLINE float GetResourceConsumptionRate() const { return BaseStatus.ResourceConsumptionRate; }
	
	FORCEINLINE float GetAttackPower() const { return BaseStatus.AttackPower; }
	FORCEINLINE float GetArmor() const { return BaseStatus.Armor; }
	
	FORCEINLINE float GetColdThreshold() const { return BaseStatus.ColdThreshold; }
	FORCEINLINE float GetHeatThreshold() const { return BaseStatus.HeatThreshold; }
	FORCEINLINE float GetHumidityThreshold() const { return BaseStatus.HumidityThreshold; }
#pragma endregion

#pragma region Add
	UFUNCTION(BlueprintCallable)
	void AddMaxHP(const float InAmount);
	UFUNCTION(BlueprintCallable)
	void AddCurrentHP(const float InAmount);
	
	void AddMaxResource(const float InAmount);
	void AddCurrentResource(const float InAmount);
	void AddResourceConsumptionRate(const float InAmount);
	
	void AddAttackPower(const float InAmount);
	void AddArmor(const float InAmount);
	void AddColdThreshold(const float InAmount);
	void AddHeatThreshold(const float InAmount);
	void AddHumidityThreshold(const float InAmount);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FBaseStats BaseStatus;
#pragma endregion
	
#pragma region Delegate
public:
	FOnHPChangedDelegate OnHPChanged;
	FOnResourceChangedDelegate OnResourceChanged;
	FOnDeathDelegate OnDeathState;
#pragma endregion
};
