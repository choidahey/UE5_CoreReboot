#pragma once

#include "CoreMinimal.h"
#include "ConsumableInventoryItemData.h"
#include "InventoryItemData.h"
#include "FriendlyAI/Data/HelperBotSaveData.h"

#include "InventorySaveData.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	General UMETA(DisplayName = "일반 아이템"),
	Consumable UMETA(DisplayName = "소모 아이템"),
	HelperBot UMETA(DisplayName = "자동화 로봇 아이템")
};

USTRUCT(BlueprintType)
struct FInventoryItemSaveData
{
	GENERATED_BODY()

	FInventoryItemSaveData()
	{
	}

	FInventoryItemSaveData(const FInventoryItemData& NewInventoryItemData, const int32 NewCount)
		: InventoryItemData(NewInventoryItemData),
		  Count(NewCount)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryItemData InventoryItemData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInventoryItemType ItemType = EInventoryItemType::General;
};

USTRUCT(BlueprintType)
struct FHelperBotItemSaveData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHelperPickUpData HelperBotPickUpData;
};

USTRUCT(BlueprintType)
struct FInventorySaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInventoryItemSaveData> ItemSaveData;
	TMap<int32, FConsumableInventoryItemData> ConsumableItemSaveData;
	TMap<int32, FHelperBotItemSaveData> HelperBotItemSaveData;
};
