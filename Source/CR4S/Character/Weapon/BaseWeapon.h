// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTool.h"
#include "GameplayTagContainer.h"
#include "Character/Data/WeaponData.h"
#include "UObject/Object.h"
#include "BaseWeapon.generated.h"

struct FBaseWeaponInfo;
class AModularRobot;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class CR4S_API ABaseWeapon : public ABaseTool
{
	GENERATED_BODY()

public:
	ABaseWeapon();
	
	virtual void Initialize(AModularRobot* OwnerCharacter);
	
#pragma region Attack
public:
	virtual float ComputeFinalDamage() override;
protected:
	void StartAttackCooldown();
	void ResetAttackCooldown();
#pragma endregion

	
#pragma region WeaponInfo
protected:
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

#pragma region Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComp;
#pragma endregion
};
