#include "BaseInventoryItem.h"

#include "CR4S.h"

UBaseInventoryItem::UBaseInventoryItem()
	: CurrentStackCount(0)
{
}

void UBaseInventoryItem::UseItem()
{
}

void UBaseInventoryItem::InitInventoryItemData(const FInventoryItemData& NewInventoryItemData,
                                              const int32 StackCount)
{
	InventoryItemData = NewInventoryItemData;
	CurrentStackCount = StackCount;
}

void UBaseInventoryItem::SetCurrentStackCount(const int32 NewStackCount)
{
	CurrentStackCount = FMath::Clamp(NewStackCount, 0, InventoryItemData.ItemInfoData.MaxStackCount);
}
