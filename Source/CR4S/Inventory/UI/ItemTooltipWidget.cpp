#include "ItemTooltipWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void UItemTooltipWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(CurrentItem))
	{
		if (IsValid(ItemIcon))
		{
			ItemIcon->SetBrushFromTexture(CurrentItem->GetItemIcon(), true);
		}

		if (IsValid(ItemName))
		{
			ItemName->SetText(CurrentItem->GetItemName());
		}

		if (IsValid(ItemDescription))
		{
			ItemDescription->SetText(CurrentItem->GetItemDescription());
		}		
	}
}

void UItemTooltipWidget::NativeDestruct()
{
	Super::NativeDestruct();

	CurrentItem = nullptr;
}

void UItemTooltipWidget::InitWidget(UBaseInventoryItem* NewItem)
{
	CurrentItem = NewItem;
}
