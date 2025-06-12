#include "RobotPartsItemSlotWidget.h"

#include "Components/Image.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void URobotPartsItemSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(BackgroundIcon))
	{
		BackgroundIcon->SetBrushFromTexture(BackgroundIconTexture);
	}
}

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
