#include "BaseItemSlotWidget.h"

#include "CR4S.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void UBaseItemSlotWidget::InitWidget(UInventoryComponent* InInventorySystemComponent, UBaseInventoryItem* NewItem)
{
	InventorySystemComponent = InInventorySystemComponent;
	CurrentItem = NewItem;

	SetItem(CurrentItem);
}

void UBaseItemSlotWidget::SetItem(UBaseInventoryItem* InItem)
{
	CurrentItem = InItem;

	if (!IsValid(CurrentItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("CurrentItem is invalid"));
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
