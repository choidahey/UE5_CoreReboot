#include "BaseInventoryItem.h"

#include "CR4S.h"

UBaseInventoryItem::UBaseInventoryItem()
	: SlotIndex(0),
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
	InventoryItemData = NewInventoryItemData;
	CurrentStackCount = StackCount;
}

void UBaseInventoryItem::SwapData(UBaseInventoryItem* OtherItem)
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(OtherItem)) ||
		CR4S_VALIDATE(LogInventory, this == OtherItem))
	{
		return;
	}
	
	Swap(InventoryItemData, OtherItem->InventoryItemData);
	Swap(CurrentStackCount, OtherItem->CurrentStackCount);
}

void UBaseInventoryItem::SetCurrentStackCount(const int32 NewStackCount)
{
	CurrentStackCount = FMath::Clamp(NewStackCount, 0, InventoryItemData.ItemInfoData.MaxStackCount);
}
