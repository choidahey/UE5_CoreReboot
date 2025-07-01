#pragma once

#include "CoreMinimal.h"
#include "InventorySaveGame.h"
#include "FriendlyAI/HelperBotState.h"
#include "HelperBotSaveGame.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FHelperBotSaveGame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BotName = FText::FromString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHelperBotState CurrentState = EHelperBotState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventorySaveGame InventoryData;

	FHelperBotSaveGame() {}

};
