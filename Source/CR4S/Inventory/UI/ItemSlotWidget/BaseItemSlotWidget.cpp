#include "BaseItemSlotWidget.h"

#include "CR4S.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void UBaseItemSlotWidget::InitWidget(UInventoryComponent* InInventoryComponent, UBaseInventoryItem* NewItem)
{
	InventoryComponent = InInventoryComponent;
	CurrentItem = NewItem;

	SetItem(CurrentItem);
}

void UBaseItemSlotWidget::SetItem(UBaseInventoryItem* InItem)
{
	CurrentItem = InItem;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)))
	{
		return;
	}

	if (CurrentItem->HasItemData())
	{
		IconImage->SetVisibility(ESlateVisibility::Visible);
		IconImage->SetBrushFromTexture(CurrentItem->GetInventoryItemData()->ItemInfoData.Info.Icon);

		if (CurrentItem->GetInventoryItemData()->ItemInfoData.MaxStackCount > 1)
		{
			CountTextBorder->SetVisibility(ESlateVisibility::Visible);
			CountTextBlock->SetText(FText::AsNumber(CurrentItem->GetCurrentStackCount()));			
		}
		else
		{
			CountTextBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		CountTextBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}
