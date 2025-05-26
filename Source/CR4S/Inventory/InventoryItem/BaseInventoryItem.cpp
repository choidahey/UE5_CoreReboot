#include "BaseInventoryItem.h"

#include "CR4S.h"

UBaseInventoryItem::UBaseInventoryItem()
	: SlotIndex(0),
	  bHasItemData(false),
	  CurrentStackCount(0)
{
}

void UBaseInventoryItem::InitInventoryItem(AActor* NewOwner, const int32 NewSlotIndex)
{
	Owner = NewOwner;
	SlotIndex = NewSlotIndex;
}

void UBaseInventoryItem::UseItem()
{
}

void UBaseInventoryItem::SetInventoryItemData(const FInventoryItemData& NewInventoryItemData,
                                              const int32 StackCount)
{
	bHasItemData = StackCount > 0;
	InventoryItemData = NewInventoryItemData;
	CurrentStackCount = StackCount;
}

void UBaseInventoryItem::SwapData(UBaseInventoryItem* OtherItem)
{
	if (!IsValid(OtherItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("OtherItem is invalid"));
		return;
	}

	if (this == OtherItem)
	{
		CR4S_Log(LogTemp, Warning, TEXT("this == OtherItem"));
		return;
	}

	Swap(bHasItemData, OtherItem->bHasItemData);
	Swap(InventoryItemData, OtherItem->InventoryItemData);
	Swap(CurrentStackCount, OtherItem->CurrentStackCount);
}

void UBaseInventoryItem::SetCurrentStackCount(const int32 NewStackCount)
{
	CurrentStackCount = FMath::Clamp(NewStackCount, 0, InventoryItemData.ItemInfoData.MaxStackCount);
	bHasItemData = CurrentStackCount > 0;
}
