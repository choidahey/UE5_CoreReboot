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
	float MaxResource{100000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Resource{100000};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceConsumptionMultiplier {1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Armor{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmorConstant {1000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower{0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdThreshold{-20};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatThreshold{30};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HumidityThreshold{100};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatDamageInterval{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatDamageAmount{10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceConsumptionAmount{1000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceRegenDelay{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceRegenInterval{0.1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceRegenPerInterval{100};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResourceRegenMultiplier{1};
};


UCLASS(Blueprintable, BlueprintType)
class CR4S_API UBaseStatusAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Base")
	FBaseStats BaseStats;
};
