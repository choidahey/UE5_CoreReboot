// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RobotSettings.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType,Blueprintable)
struct CR4S_API FRobotSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 MappingContextPriority{2};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	FVector UnMountLocation {FVector(-200.f,0.f,0.f)};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mount")
	FName MountSocketName {FName("cockpit")};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashStrength{3000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashCooldown{1};
};

USTRUCT(BlueprintType,Blueprintable)
struct CR4S_API FWeaponSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName AttachSocketName{};
	
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
