#include "BaseItemSlotWidget.h"

#include "CR4S.h"
#include "DummyItemSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void UBaseItemSlotWidget::InitWidget(UBaseInventoryItem* NewItem, const bool bNewCanDrag, const bool bNewCanDrop)
{
	CurrentItem = NewItem;

	SetItem(CurrentItem);

	bCanDrag = bNewCanDrag;
	bCanDrop = bNewCanDrop;	
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

void UBaseItemSlotWidget::EmptyItem()
{
	CurrentItem = nullptr;
}

FReply UBaseItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, CurrentItem->HasItemData()))
	{
		return FReply::Unhandled();
	}

	CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnMouseButtonDown: %d"), CurrentItem->GetSlotIndex());

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UBaseItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnDragDetected"));

	if (!CR4S_VALIDATE(LogInventoryUI, bCanDrag))
	{
		return;
	}

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(DummySlotWidgetClass)))
	{
		return;
	}

	UDragDropOperation* DragOperation = NewObject<UDragDropOperation>();
	DragOperation->Payload = this;

	UDummyItemSlotWidget* DummySlotWidgetInstance = CreateWidget<UDummyItemSlotWidget>(
		GetWorld(), DummySlotWidgetClass);

	DummySlotWidgetInstance->SetDummy(CurrentItem->GetInventoryItemData()->ItemInfoData.Info.Icon,
	                                  CurrentItem->GetCurrentStackCount());

	DragOperation->DefaultDragVisual = DummySlotWidgetInstance;

	OutOperation = DragOperation;
}

bool UBaseItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnDrop"));

	if (!CR4S_VALIDATE(LogInventoryUI, bCanDrop))
	{
		return false;
	}

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InOperation)))
	{
		return false;
	}

	UBaseItemSlotWidget* FromSlot = Cast<UBaseItemSlotWidget>(InOperation->Payload);
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
			const int32 ToItemCount = CurrentItem->GetCurrentStackCount();
			const int32 MaxStackCount = CurrentItem->GetInventoryItemData()->ItemInfoData.MaxStackCount;
			if (ToItemCount < MaxStackCount)
			{
				const int32 CanAddCount = MaxStackCount - ToItemCount;
				const int32 ActualAddCount = FMath::Min(CanAddCount, FromItem->GetCurrentStackCount());

				CurrentItem->SetCurrentStackCount(ToItemCount + ActualAddCount);
				FromItem->SetCurrentStackCount(FromItem->GetCurrentStackCount() - ActualAddCount);
			}
		}
		else
		{
			FromItem->SwapData(CurrentItem);
		}

		FromSlot->SetItem(FromItem);
		SetItem(CurrentItem);

		return true;
	}

	return false;
}
