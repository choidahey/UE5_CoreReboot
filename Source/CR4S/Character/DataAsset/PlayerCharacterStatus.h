// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Settings/AlsInAirRotationMode.h"
#include "PlayerCharacterStatus.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FPlayerCharacterStats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health{4000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hunger{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack{0};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Armor{50};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Stamina{10000};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdThreshold{-20};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatThreshold{30};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HumidityThreshold{100};
	
};

UCLASS(Blueprintable, BlueprintType)
class CR4S_API UPlayerCharacterStatus : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerCharacterStats StatsData;
	
};
