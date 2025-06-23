// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusAsset.h"
#include "Engine/DataAsset.h"
#include "ModularRobotStatus.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FModularRobotStats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxEnergy{30000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Energy{30000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyConsumptionAmount{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyConsumptionInterval{0.1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoverResourceCost{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoverResourceConsumptionInterval{0.1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdResourceConsumptionMultiplier {1.2};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStun{1000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stun{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunResistance{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunDuration{1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmorMultiplier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPowerMultiplier{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HighHumidityAttackPowerMultiplier{0.9};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxWeight{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{0};
};

UCLASS()
class CR4S_API UModularRobotStatusAsset : public UBaseStatusAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FModularRobotStats RobotStats;
};
