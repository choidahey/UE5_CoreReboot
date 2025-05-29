#include "PlayerInventoryWidget.h"

#include "CR4S.h"
#include "Inventory/UI/InventoryContainerWidget.h"

FReply UPlayerInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent)) ||
		InKeyEvent.GetKey() != EKeys::R)
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	InventoryComponent->SortInventoryItems();
	return FReply::Handled();
}
