// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CR4S_API UMeleeWeapon : public UBaseWeapon
{
	GENERATED_BODY()

public:
	UMeleeWeapon();

#pragma region Override
public:
	virtual void OnAttack(const int32 WeaponIdx) override;
	virtual void Initialize(AModularRobot* OwnerCharacter) override;
protected:
#pragma endregion
	
#pragma region TypeSpecificInfo
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMeleeWeaponInfo TypeSpecificInfo; 
#pragma endregion
};
