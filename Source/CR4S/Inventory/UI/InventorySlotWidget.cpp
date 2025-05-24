#include "InventorySlotWidget.h"

#include "CR4S.h"
#include "InventoryDummySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

bool UInventorySlotWidget::Initialize()
{
	CurrentItem = nullptr;
	return Super::Initialize();
}

void UInventorySlotWidget::InitWidget(UInventorySystemComponent* InInventorySystemComponent, UBaseInventoryItem* NewItem)
{
	InventorySystemComponent = InInventorySystemComponent;
	CurrentItem = NewItem;

	SetItem(CurrentItem);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsValid(CurrentItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("CurrentItem is invalid"));
		return FReply::Unhandled();
	}

	if (!CurrentItem->HasItemData())
	{
		CR4S_Log(LogTemp, Warning, TEXT("CurrentItem has no item data"));
		return FReply::Unhandled();
	}
		
	UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseButtonDown: %d"), CurrentItem->GetSlotIndex());
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDragDetected"));

	if (!IsValid(CurrentItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("CurrentItem is invalid"));
		return;
	}
	
	if (IsValid(DummySlotWidgetClass))
	{
		UDragDropOperation* DragOperation = NewObject<UDragDropOperation>();
		DragOperation->Payload = this;
		
		UInventoryDummySlotWidget* DummySlotWidgetInstance = CreateWidget<UInventoryDummySlotWidget>(GetWorld(), DummySlotWidgetClass);
		
		DummySlotWidgetInstance->SetDummy(CurrentItem->GetIcon(), CurrentItem->GetCurrentStackCount());
	
		DragOperation->DefaultDragVisual = DummySlotWidgetInstance;
		
		OutOperation = DragOperation;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DummySlotWidgetClass is invalid"));		
	}
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnDrop"));

	if (!IsValid(CurrentItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("CurrentItem is invalid"));
		return false;
	}

	const UInventorySlotWidget* FromSlot = Cast<UInventorySlotWidget>(InOperation->Payload);
	if (!IsValid(FromSlot))
	{
		CR4S_Log(LogTemp, Warning, TEXT("FromSlot is invalid"));
		return false;
	}

	UBaseInventoryItem* FromItem = FromSlot->CurrentItem;
	if (!IsValid(FromItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("FromItem is invalid"));
		return false;
	}
		
	if (FromItem != CurrentItem)
	{
		// Same Item
		if (FromItem->GetInventoryItemData()->RowName == CurrentItem->GetInventoryItemData()->RowName)
		{
			InventorySystemComponent->MergeItems(FromItem, CurrentItem);
		}
		else
		{
			InventorySystemComponent->SwapItems(FromItem, CurrentItem);
		}
				
		return true;
	}

	return false;
}

void UInventorySlotWidget::SetItem(UBaseInventoryItem* InItem)
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
		CountTextBorder->SetVisibility(ESlateVisibility::Visible);

		IconImage->SetBrushFromTexture(CurrentItem->GetIcon());
		CountTextBlock->SetText(FText::AsNumber(CurrentItem->GetCurrentStackCount()));
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		CountTextBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}
