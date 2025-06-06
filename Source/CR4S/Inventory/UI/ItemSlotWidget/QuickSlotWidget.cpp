#include "QuickSlotWidget.h"

#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"

bool UQuickSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const bool bResult = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UnEquipItem(InventoryComponent, SlotIndex);
	
	return bResult;
}

FReply UQuickSlotWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!IsValid(CurrentItem) ||
		!IsValid(InventoryComponent) ||
		!IsValid(InventoryContainerWidget))
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}
	
	const int32 QuickSlotIndex = QuickSlotKeys.IndexOfByKey(InKeyEvent.GetKey());
	if (QuickSlotIndex != INDEX_NONE)
	{
		UnEquipItem(InventoryComponent, SlotIndex);
		InventoryComponent->SwapItem(InventoryComponent, SlotIndex, QuickSlotIndex);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
