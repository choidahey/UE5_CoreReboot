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
class CR4S_API URangedWeapon : public UBaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	URangedWeapon();
	
#pragma region Override
public:
	virtual void OnAttack(const int32 WeaponIdx) override;
	virtual void Initialize(AModularRobot* OwnerCharacter) override;
protected:
#pragma endregion
	
#pragma region TypeSpecificInfo
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRangedWeaponInfo TypeSpecificInfo; 
#pragma endregion
	
};
