// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/Data/WeaponData.h"
#include "UObject/Object.h"
#include "BaseWeapon.generated.h"

struct FBaseWeaponInfo;
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

#pragma region GetSet
	FORCEINLINE FGameplayTag GetGameplayTag() const { return WeaponTag; }
	
	void SetGameplayTag(const FGameplayTag GameplayTag);
#pragma endregion

	virtual void Initialize(AModularRobot* OwnerCharacter);
	
#pragma region Attack
public:
	virtual void OnAttack();
	float ComputeFinalDamage();
protected:
	void StartAttackCooldown();
	void ResetAttackCooldown();
#pragma endregion

	
#pragma region WeaponInfo
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FGameplayTag WeaponTag;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FBaseWeaponInfo BaseInfo;
#pragma endregion

#pragma region Cached
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	TObjectPtr<AModularRobot> OwningCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	uint8 bCanAttack:1 {true};

	FTimerHandle AttackCooldownTimerHandler;	
#pragma endregion
};
