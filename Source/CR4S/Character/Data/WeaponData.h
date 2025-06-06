// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Utility/Cr4sGameplayTags.h"
#include "WeaponData.generated.h"

class ABaseBullet;
/**
 * 
 */
USTRUCT(BlueprintType)
struct CR4S_API FBulletInfo
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLifeTime{3};
};

USTRUCT(BlueprintType)
struct CR4S_API FBaseWeaponInfo
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunAmount{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCooldownTime{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bHasSelfStun:1 {false};
};


USTRUCT(BlueprintType)
struct CR4S_API FMeleeWeaponInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCombo{1};
};

USTRUCT(BlueprintType)
struct CR4S_API FRangedWeaponInfo
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
	TSubclassOf<ABaseBullet> ProjectileClass {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBulletInfo BulletInfo;
};

UCLASS(BlueprintType)
class CR4S_API UWeaponInfoDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	TMap<FGameplayTag,FBaseWeaponInfo> BaseInfo
	{
		{ WeaponTags::Chainsaw,{} },
		{ WeaponTags::DemolitionGear,{} },
		{ WeaponTags::ShockBat,{} },
		{ WeaponTags::CrystalSword,{} },
		{ WeaponTags::CrystalShotgun,{} },
		{ WeaponTags::CrystalRifle,{} },
		{ WeaponTags::CrystalBurstRifle,{} },
		{ WeaponTags::CrystalGatling,{} },
		{ WeaponTags::CrystalSMG,{} },
		{ WeaponTags::CrystalLauncher2,{} },
		{ WeaponTags::CrystalHomingLauncher4,{} },
		{ WeaponTags::CrystalHomingHighSpeed4,{} },
		{ WeaponTags::Fireball,{} },
		{ WeaponTags::HomingFireball,{} },
		{ WeaponTags::IceShotgun,{} },
		{ WeaponTags::ThunderStrike,{} },
		{ WeaponTags::Comet,{} }
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Melee")
	TMap<FGameplayTag,FMeleeWeaponInfo> MeleeInfo
	{
			{ WeaponTags::Chainsaw,{} },
			{ WeaponTags::DemolitionGear,{} },
			{ WeaponTags::ShockBat,{} },
			{ WeaponTags::CrystalSword,{} },
	};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ranged")
	TMap<FGameplayTag,FRangedWeaponInfo> RangedInfo
	{
			{ WeaponTags::CrystalShotgun,{} },
			{ WeaponTags::CrystalRifle,{} },
			{ WeaponTags::CrystalBurstRifle,{} },
			{ WeaponTags::CrystalGatling,{} },
			{ WeaponTags::CrystalSMG,{} },
			{ WeaponTags::CrystalLauncher2,{} },
			{ WeaponTags::CrystalHomingLauncher4,{} },
			{ WeaponTags::CrystalHomingHighSpeed4,{} },
			{ WeaponTags::Fireball,{} },
			{ WeaponTags::HomingFireball,{} },
			{ WeaponTags::IceShotgun,{} },
			{ WeaponTags::ThunderStrike,{} },
			{ WeaponTags::Comet,{} }
	};
};
