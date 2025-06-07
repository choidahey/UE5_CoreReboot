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

public:
	virtual void OnAttack(const int32 WeaponIdx) override;
};
