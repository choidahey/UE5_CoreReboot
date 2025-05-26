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
	if (!IsValid(DataTable))
	{
		CR4S_Log(LogTemp, Warning, TEXT("DataTable is invalid"));
		return;
	}

	if (const FToolItemData* FindItemData =
		DataTable->FindRow<FToolItemData>(NewInventoryItemData.RowName, TEXT("")))
	{
		ToolItemData = *FindItemData;
	}
}
