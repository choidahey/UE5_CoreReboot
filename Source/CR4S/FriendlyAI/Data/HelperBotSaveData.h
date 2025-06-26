#pragma once

#include "CoreMinimal.h"
#include "FriendlyAI/HelperBotState.h"
#include "HelperBotSaveData.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FHelperPickUpData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BotName = FText::FromString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 0.0f;

	FHelperPickUpData()	{}
};