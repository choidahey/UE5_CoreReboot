// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ModularRobotStatus.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FModularRobotStatus
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float Health{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float HeatThreshold{80};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float MaxStun{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float Temperature{30};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float Humidity{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float MaxHumidity{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float HumidityThreshold{50};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float Armor{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float ArmorMultiplier{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float AttackPower{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float Weight{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float MaxWeight{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModularRobot")
	float StunResistance{0};
};

UCLASS()
class CR4S_API UModularRobotStatus : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	FModularRobotStatus Status;
};
