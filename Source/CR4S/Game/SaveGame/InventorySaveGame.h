#pragma once

#include "CoreMinimal.h"
#include "FriendlyAI/Data/HelperBotSaveData.h"
#include "Inventory/Data/ConsumableItemSaveGame.h"
#include "Inventory/Data/InventoryItemData.h"

#include "InventorySaveGame.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemSaveGame
{
	GENERATED_BODY()

	FInventoryItemSaveGame()
	{
	}

	FInventoryItemSaveGame(const FInventoryItemData& NewInventoryItemData,
		const int32 NewCount)
		: InventoryItemData(NewInventoryItemData),
		  Count(NewCount)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryItemData InventoryItemData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFreshnessInfo FreshnessInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHelperPickUpData HelperBotItemData;
};

USTRUCT(BlueprintType)
struct FInventorySaveGame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryItemSaveGame> InventoryItemSaveGame;
};
