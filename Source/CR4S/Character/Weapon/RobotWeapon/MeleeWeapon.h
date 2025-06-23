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
	virtual void OnAttack() override;
	virtual void Initialize(AModularRobot* OwnerCharacter) override;
protected:
#pragma endregion
	
#pragma region TypeSpecificInfo
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMeleeWeaponInfo TypeSpecificInfo; 
#pragma endregion
};
