#include "PlayerInventoryWidget.h"

#include "CR4S.h"
#include "Inventory/UI/InventoryContainerWidget.h"

FReply UPlayerInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	CR4S_Log(LogInventoryUI, Warning, TEXT("Key: %s"), *InKeyEvent.GetKey().ToString());
	
	if (InKeyEvent.GetKey() == EKeys::G && CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent)))
	{
		InventoryComponent->SortInventoryItems();
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}