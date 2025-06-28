// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RobotSettings.generated.h"

/**
 * 
 */

class UNiagaraSystem;

USTRUCT(BlueprintType,Blueprintable)
struct CR4S_API FRobotSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 MovementMappingContextPriority{2};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 UtilityMappingContextPriority{2};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	float UnMountOffset {-200};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	FName MountSocketName {FName("cockpit")};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	int32 CollisionCheckCount{8};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float BoosterStrength{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float LegStrength{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashZMultiplier{0.3};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashCooldown{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float WeightFactor{0.1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed{600};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpZVelocity{700};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSlopeAngle{10};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovere")
	TObjectPtr<UCurveFloat> HoverCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hovere")
	float HoverUpwardSpeed{30};

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> DashEffect;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USoundBase> DashSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> HoverEffect;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USoundBase> HoverSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> BoosterSocketNames{FName(),FName()};
};

USTRUCT(BlueprintType,Blueprintable)
struct CR4S_API FWeaponSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<FName> AttachSocketNames;
	
};

UCLASS()
class CR4S_API URobotSettingsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRobotSettings RobotSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponSettings WeaponSettings;
};
