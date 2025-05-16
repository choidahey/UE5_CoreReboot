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
	float Health{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Hunger{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stamina{100};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdThreshold{30};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatThreshold{40};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Temperature{36};
	
};

UCLASS(Blueprintable, BlueprintType)
class CR4S_API UPlayerCharacterStatus : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerCharacterStats StatsData;
	
};
