#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/ItemData.h"

#include "InventoryItemData.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	FInventoryItemData()
		: SlotIndex(0)
	{
	}

	FInventoryItemData(const int32 InSlotIndex,
					   const FName InRowName,
					   const FItemInfoData& InItemInfoData)
		: SlotIndex(InSlotIndex),
		  RowName(InRowName),
		  ItemInfoData(InItemInfoData)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SlotIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName RowName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FItemInfoData ItemInfoData;

	bool IsStackableItem() const
	{
		return ItemInfoData.MaxStackCount > 1;
	}
};