// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/PlayerCharacterStatus.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacterStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHungerChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedDelegate, float /*InPercentage*/)
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);

class UPlayerCharacterStatus;
class APlayerCharacter;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UPlayerCharacterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCharacterStatusComponent();
#pragma region Get
	FORCEINLINE int32 GetMaxHP() const { return Status.MaxHealth; }
	FORCEINLINE int32 GetCurrentHP() const { return Status.Health; }
	FORCEINLINE int32 GetMaxHunger() const { return Status.MaxHunger; }
	FORCEINLINE int32 GetCurrentHunger() const { return Status.Hunger; }
	FORCEINLINE int32 GetMaxStamina() const { return Status.MaxStamina; }
	FORCEINLINE int32 GetCurrentStamina() const { return Status.Stamina; }
	FORCEINLINE int32 GetAttackPower() const { return Status.AttackPower; }
	FORCEINLINE int32 GetArmor() const { return Status.Armor; }
	FORCEINLINE int32 GetColdThreshold() const { return Status.ColdThreshold; }
	FORCEINLINE int32 GetHeatThreshold() const { return Status.HeatThreshold; }
	FORCEINLINE int32 GetHumidityThreshold() const { return Status.HumidityThreshold; }
#pragma endregion

#pragma region Add
	UFUNCTION(BlueprintCallable)
	void AddMaxHP(const int32 InAmount);
	UFUNCTION(BlueprintCallable)
	void AddCurrentHP(const int32 InAmount);
	void AddMaxHunger(const int32 InAmount);
	void AddCurrentHunger(const int32 InAmount);
	void AddMaxStamina(const int32 InAmount);
	void AddCurrentStamina(const int32 InAmount);
	void AddAttackPower(const int32 InAmount);
	void AddArmor(const int32 InAmount);
	void AddColdThreshold(const int32 InAmount);
	void AddHeatThreshold(const int32 InAmount);
	void AddHumidityThreshold(const int32 InAmount);
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
	TObjectPtr<UPlayerCharacterStatus> StatusData; 
#pragma endregion

#pragma region Owner
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Owner")
	TObjectPtr<APlayerCharacter> OwningCharacter;
#pragma endregion
	
#pragma region Status
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FPlayerCharacterStats Status;
#pragma endregion

#pragma region Delegate
public:
	FOnHPChangedDelegate OnHPChanged;
	FOnHungerChangedDelegate OnHungerChanged;
	FOnStaminaChangedDelegate OnStaminaChanged;
	FOnDeathDelegate OnDeathState;
#pragma endregion
};
