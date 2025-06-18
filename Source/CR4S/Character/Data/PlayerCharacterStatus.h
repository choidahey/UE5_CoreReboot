// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusAsset.h"
#include "Engine/DataAsset.h"
#include "PlayerCharacterStatus.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FPlayerCharacterStats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHunger{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Hunger{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HungerInterval{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HungerDecreaseAmount{0.1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HungerConsumptionMultiplier{1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StarvationDamage{1};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StarvationDamageInterval{1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SprintResourceCost{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SprintCostConsumptionInterval{0.1};

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// float ColdSpeedMultiplier{0.5};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdResourceRegenMultiplier{0.8};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdHungerConsumptionMultiplier{1.1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMontagePlayRate{1.3};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HighHumidityMontagePlayRate{1};
};

UCLASS(Blueprintable, BlueprintType)
class CR4S_API UPlayerCharacterStatusAsset : public UBaseStatusAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
	FPlayerCharacterStats PlayerStats;
	
};
