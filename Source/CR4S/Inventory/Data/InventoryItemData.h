#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/ItemData.h"

#include "InventoryItemData.generated.h"

UENUM(BlueprintType)
enum class EInventoryItemType : uint8
{
	General UMETA(DisplayName = "일반 아이템"),
	Consumable UMETA(DisplayName = "소모 아이템"),
	HelperBot UMETA(DisplayName = "자동화 로봇 아이템")
};

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	FInventoryItemData()
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
	EInventoryItemType ItemType = EInventoryItemType::General;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SlotIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName RowName = NAME_None;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FItemInfoData ItemInfoData;

	bool IsStackableItem() const
	{
		return ItemInfoData.MaxStackCount > 1;
	}
};
