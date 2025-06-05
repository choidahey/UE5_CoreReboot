// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "BaseWeapon.generated.h"

class AModularRobot;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class CR4S_API UBaseWeapon : public UObject
{
	GENERATED_BODY()

public:
	UBaseWeapon();

#pragma region Get
	FORCEINLINE float GetDamageMultiplier() const { return DamageMultiplier; }
	FORCEINLINE float GetStunAmount() const { return StunAmount; }
	FORCEINLINE float GetWeight() const { return Weight; }
#pragma endregion

#pragma region Set
	void SetGameplayTag(const FGameplayTag GameplayTag);
#pragma endregion

	void Initialize(AModularRobot* OwnerCharacter);

	UFUNCTION(BlueprintCallable)
	virtual void LoadFromDataTable();
	
#pragma region Attack
	virtual void OnAttack(const int32 WeaponIdx);
#pragma endregion

	
#pragma region Properties
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FGameplayTag WeaponTag;
	
	float DamageMultiplier;
	float StunAmount;
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> AttackMontage;
#pragma endregion

#pragma region Owener
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	TObjectPtr<AModularRobot> OwningCharacter;
#pragma endregion
};
