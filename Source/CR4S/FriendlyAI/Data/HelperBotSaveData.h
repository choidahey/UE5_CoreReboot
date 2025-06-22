#pragma once

#include "CoreMinimal.h"
#include "HelperBotSaveData.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FHelperBotSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BotName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CurrentRotation = FRotator::ZeroRotator;

	FHelperBotSaveData()
	{
		BotName = TEXT("");
		CurrentHealth = 0.0f;
		CurrentLocation = FVector::ZeroVector;
		CurrentRotation = FRotator::ZeroRotator;
	}
};

USTRUCT(BlueprintType)
struct CR4S_API FHelperPickUpData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BotName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 0.0f;

	FHelperPickUpData()
	{
		BotName = TEXT("");
		CurrentHealth = 0.0f;
	}
};