// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CR4S_API AMeleeWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	AMeleeWeapon();

#pragma region Get
	FORCEINLINE float GetStunAmount() const { return TypeSpecificInfo.StunAmount; }
#pragma endregion

	
#pragma region Override
public:
	virtual void Initialize(AModularRobot* OwnerCharacter, int32 SlotIdx) override;
protected:
#pragma endregion
	
#pragma region Cached
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMeleeWeaponInfo TypeSpecificInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPressTime {0};
#pragma endregion
};
