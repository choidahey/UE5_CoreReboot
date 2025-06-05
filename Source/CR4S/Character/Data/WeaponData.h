// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.generated.h"
class ABaseBullet;
/**
 * 
 */
USTRUCT(BlueprintType)
struct CR4S_API FBulletData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UParticleSystem> ImpactParticle;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InitialBulletSpeed{2000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxBulletSpeed{2000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HomingStrength{0};
};

USTRUCT(BlueprintType)
struct CR4S_API FBaseWeaponData
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
	uint8 bHasSelfStun:1 {false};
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
	int32 MagazineCapacity{10};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo{10};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate{5};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime{2};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Recoil{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LockOnTime{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BulletPerShot{1};
	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (EditCondition = "BulletsPerShot > 1"))
	float SpreadAngle = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseBullet> ProjectileClass{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBulletData BulletData;
};
