// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Utility/Cr4sGameplayTags.h"
#include "RobotPartsData.generated.h"


class UNiagaraSystem;
struct FMeleeWeaponInfo;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct CR4S_API FCorePartsInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmorMultiplier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdditiveTemperatureThreshold{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdditiveHumidityThreshold{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxResource{10000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyConsumptionAmount{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStun{1000};
};

USTRUCT(BlueprintType)
struct CR4S_API FBodyPartsInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth{2400};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Armor{330};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStun{350};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{470};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyEfficiency{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceRegenModifier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceRegenDelay{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceConsumptionModifier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatThreshold{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdThreshold{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HumidityThreshold{0};
};

USTRUCT(BlueprintType)
struct CR4S_API FArmPartsInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth{1500};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Armor{190};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{450};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxArmLoad{500};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoilModifier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeleeDamageModifier{1};
};

USTRUCT(BlueprintType)
struct CR4S_API FLegPartsInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth{3500};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Armor{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStun{500};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{500};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTotalWeight{2000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWalkSpeed{700};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LegStrength{5000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSlopeAngle{30};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyEfficiency{1};
};

USTRUCT(BlueprintType)
struct CR4S_API FBoosterPartsInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoosterStrength{20000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceConsumption{2000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashCooldown{1};
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> DashEffect;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USoundBase> DashSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> HoverEffect;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USoundBase> HoverSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoosterSocketName{};
};

UCLASS()
class CR4S_API UCorePartsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Core")
	TMap<FGameplayTag,FCorePartsInfo> CoreInfo
	{
					{ CoreTags::Tier1,{} },
					{ CoreTags::Tier2,{} },
					{ CoreTags::Tier3,{} },
					{ CoreTags::Tier4,{} },
		};
};

UCLASS()
class CR4S_API UBodyPartsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Body")
	TMap<FGameplayTag,FBodyPartsInfo> BodyInfo
	{
					{ BodyTags::BalanceLight,{} },
					{ BodyTags::BalanceHeavy,{} },
					{ BodyTags::HumidityStandard,{} },
					{ BodyTags::TemperatureStandard,{} }
	};
};

UCLASS()
class CR4S_API UArmPartsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arm")
	TMap<FGameplayTag,FArmPartsInfo> ArmInfo
	{
					{ ArmTags::BalanceHeavy,{} },
					{ ArmTags::MeleeLight,{} },
					{ ArmTags::RangedStandard,{} }
	};
};

UCLASS()
class CR4S_API ULegPartsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leg")
	TMap<FGameplayTag,FLegPartsInfo> LegInfo
	{
					{ LegTags::QuadrupedalLight,{} },
					{ LegTags::BipedalStandard,{} },
					{ LegTags::ReverseBipedalLight,{} },
					{ LegTags::QuadrupedalHeavy,{} },
					{ LegTags::ReverseBipedalLight,{} }
	};
};

UCLASS()
class CR4S_API UBoosterPartsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Booster")
	TMap<FGameplayTag,FBoosterPartsInfo> BoosterInfo
	{
					{ BoosterTags::Cyclic,{} },
					{ BoosterTags::HighPower,{} },
					{ BoosterTags::LongRanged,{} },
					{ BoosterTags::Recovery,{} },
					{ BoosterTags::Standard,{} },
					{ BoosterTags::Sustained,{} }
	};
};
