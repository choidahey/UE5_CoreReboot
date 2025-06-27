// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Utility/Cr4sGameplayTags.h"
#include "WeaponData.generated.h"

class ABaseWeapon;
class APlayerTool;
class ABaseTool;
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
	float HomingActivationDelay{0.2f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLifeTime{3};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunAmount{0};
};

USTRUCT(BlueprintType)
struct CR4S_API FBaseWeaponInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UStaticMesh>> SkeletalMeshs {nullptr, nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages {nullptr, nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier{1};
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
	TObjectPtr<UAnimMontage> ChargeAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargeAttackTimeThreshold{0.5};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunAmount{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCombo{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TopSocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BottomSocketName{};
};

USTRUCT(BlueprintType)
struct CR4S_API FAmmoInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity{10};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentAmmo{10};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime{2};
};

USTRUCT(BlueprintType)
struct CR4S_API FRecoilInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Recoil{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HorizontalRecoilMultiplier{0.15};
};

USTRUCT(BlueprintType)
struct CR4S_API FSpreadShotInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BulletPerShot{16};
	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (EditCondition = "BulletPerShot > 1"))
	float SpreadAngle{20.f};
};

USTRUCT(BlueprintType)
struct CR4S_API FMultiShotInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> MuzzleSocketNames;
};

USTRUCT(BlueprintType)
struct CR4S_API FHomingInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LockOnTime{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0"))
	float LockOnMaintainRadius{0.2};
};

USTRUCT(BlueprintType)
struct CR4S_API FBurstShotInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ShotsPerBurst{3};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBetweenShots{0.05};
};

USTRUCT(BlueprintType)
struct CR4S_API FAutomaticFireInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ShotsPerSecond{20};
};

USTRUCT(BlueprintType)
struct CR4S_API FRangedWeaponInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	float MaxAimTrackingRange{10000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	FName MuzzleSocketName{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	TSubclassOf<ABaseBullet> ProjectileClass {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	FBulletInfo BulletInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | Ammo")
	FAmmoInfo AmmoInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | Recoil")
	FRecoilInfo RecoilInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | SpreadShot")
	FSpreadShotInfo SpreadShotInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | MultiShot")
	FMultiShotInfo MultiShotInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | BurstShot")
	FBurstShotInfo BurstShotInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | AutomaticFire")
	FAutomaticFireInfo AutomaticFireInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Feature | Homing")
	FHomingInfo HomingInfo;
};

USTRUCT(BlueprintType)
struct CR4S_API FPlayerToolInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> StaticMesh{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower{100};
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

UCLASS(BlueprintType)
class CR4S_API UWeaponClassDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotWeapon")
	TMap<FGameplayTag,TSubclassOf<ABaseWeapon>> WeaponClassData
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
};

UCLASS(BlueprintType)
class CR4S_API UPlayerToolInfoDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerTool")
	TMap<FGameplayTag,FPlayerToolInfo> PlayerToolInfo
	{
				{ToolTags::Axe,{}},
				{ToolTags::PickAxe,{}},
				{ToolTags::Hammer,{}}
	};
};