// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct CR4S_API FBaseWeaponData : public FTableRowBase 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunAmount{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bHasRecoil:1 {false};
};

USTRUCT(BlueprintType)
struct CR4S_API FMeleeWeaponData : public FBaseWeaponData 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCombo{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCoolDown{1};
};

USTRUCT(BlueprintType)
struct CR4S_API FRangedWeaponData : public FBaseWeaponData 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range{10000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate{5};
};
