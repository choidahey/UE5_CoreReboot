#include "RobotPartsItemSlotWidget.h"

#include "Inventory/InventoryItem/BaseInventoryItem.h"

bool URobotPartsItemSlotWidget::IsItemAllowedByFilter(UBaseInventoryItem* Item) const
{
	const bool Result = Super::IsItemAllowedByFilter(Item);
	if (Result)
	{
		if (IsValid(Item))
		{
			const FGameplayTagContainer& RobotPartsTag = Item->GetItemTags();
			return RobotPartsTag.HasTag(PartsTypeTag);
		}
	}
	
	return Result;
}
