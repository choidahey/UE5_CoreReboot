// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseStatusAsset.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FBaseStats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth{4000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health{4000};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxEnergy{100000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Energy{100000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ForceUnits="Energy/s"))
	float EnergyConsumptionRate{200};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Armor{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower{0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdThreshold{-20};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatThreshold{30};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HumidityThreshold{100};
};


UCLASS(Blueprintable, BlueprintType)
class CR4S_API UBaseStatusAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	FBaseStats BaseStats;
};
