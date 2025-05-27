#include "ToolInventoryItem.h"

#include "CR4S.h"

void UToolInventoryItem::UseItem()
{
	Super::UseItem();
}

void UToolInventoryItem::SetInventoryItemData(const FInventoryItemData& NewInventoryItemData,
                                              const int32 StackCount)
{
	Super::SetInventoryItemData(NewInventoryItemData, StackCount);

	const UDataTable* DataTable = NewInventoryItemData.ItemInfoData.DetailData.DataTable;
	if (!CR4S_VALIDATE(LogInventory, IsValid(DataTable)))
	{
		return;
	}

	if (const FToolItemData* FindItemData =
		DataTable->FindRow<FToolItemData>(NewInventoryItemData.RowName, TEXT("")))
	{
		ToolItemData = *FindItemData;
	}
}
