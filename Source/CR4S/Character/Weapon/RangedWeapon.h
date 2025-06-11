// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Character/Data/WeaponData.h"
#include "RangedWeapon.generated.h"

struct FRangedWeaponData;
class ABaseBullet;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CR4S_API ARangedWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARangedWeapon();
	
#pragma region Override
public:
	virtual void OnAttack() override;
	virtual void Initialize(AModularRobot* OwnerCharacter) override;
protected:
#pragma endregion
	
#pragma region TypeSpecificInfo
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRangedWeaponInfo TypeSpecificInfo; 
#pragma endregion
	
};
