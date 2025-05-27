#include "InventorySlotWidget.h"

#include "CR4S.h"
#include "InventoryDummySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, CurrentItem->HasItemData()))
	{
		return FReply::Unhandled();
	}

	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnMouseButtonDown: %d"), CurrentItem->GetSlotIndex());

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnDragDetected"));

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(DummySlotWidgetClass)))
	{
		return;
	}

	UDragDropOperation* DragOperation = NewObject<UDragDropOperation>();
	DragOperation->Payload = this;

	UInventoryDummySlotWidget* DummySlotWidgetInstance = CreateWidget<UInventoryDummySlotWidget>(
		GetWorld(), DummySlotWidgetClass);

	DummySlotWidgetInstance->SetDummy(CurrentItem->GetInventoryItemData()->ItemInfoData.Info.Icon,
									  CurrentItem->GetCurrentStackCount());

	DragOperation->DefaultDragVisual = DummySlotWidgetInstance;

	OutOperation = DragOperation;
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnDrop"));
	
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InOperation)))
	{
		return false;
	}

	const UInventorySlotWidget* FromSlot = Cast<UInventorySlotWidget>(InOperation->Payload);
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(FromSlot)))
	{
		return false;
	}

	UBaseInventoryItem* FromItem = FromSlot->CurrentItem;
	if (!CR4S_VALIDATE(LogInventoryUI, FromItem))
	{
		return false;
	}

	if (FromItem != CurrentItem)
	{
		// Same Item
		if (FromItem->GetInventoryItemData()->RowName == CurrentItem->GetInventoryItemData()->RowName)
		{
			InventoryComponent->MergeItems(FromItem, CurrentItem);
		}
		else
		{
			InventoryComponent->SwapItems(FromItem, CurrentItem);
		}

		return true;
	}

	return false;
}
