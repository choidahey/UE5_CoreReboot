// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "RangedWeapon.generated.h"

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
protected:
#pragma endregion
	
#pragma region Projectile
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<ABaseBullet> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Range;
#pragma endregion
	
};
