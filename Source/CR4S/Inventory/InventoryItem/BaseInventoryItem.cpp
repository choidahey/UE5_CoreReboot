#include "BaseInventoryItem.h"

#include "CR4S.h"

UBaseInventoryItem::UBaseInventoryItem()
	: SlotIndex(0),
	  bHasItemData(false),
	  Icon(nullptr),
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

void UBaseInventoryItem::SetInventoryItemData(const FInventoryItemData& NewInventoryItemData, UTexture2D* NewIcon,
                                              const int32 StackCount)
{
	bHasItemData = StackCount > 0;
	InventoryItemData = NewInventoryItemData;
	Icon = NewIcon;
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
	Swap(Icon, OtherItem->Icon);
	Swap(CurrentStackCount, OtherItem->CurrentStackCount);
}

void UBaseInventoryItem::SetCurrentStackCount(const int32 NewStackCount)
{
	CurrentStackCount = FMath::Clamp(NewStackCount, 0, InventoryItemData.MaxStackCount);
	bHasItemData = CurrentStackCount > 0;
}
