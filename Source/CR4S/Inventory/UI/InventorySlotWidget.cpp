#include "InventorySlotWidget.h"

#include "InventoryDummySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

bool UInventorySlotWidget::Initialize()
{
	SlotIndex = 0;
	
	return Super::Initialize();
}

void UInventorySlotWidget::InitWidget(UInventorySystemComponent* InInventorySystemComponent, const int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	InventorySystemComponent = InInventorySystemComponent;

	if (const FInventoryItem* Item = InventorySystemComponent->GetItemDataByIndex(SlotIndex))
	{
		SetItem(Item->Icon, Item->Count);
	}
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseButtonDown: %d"), SlotIndex);
	
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

	if (IsValid(DummySlotWidgetClass))
	{
		UDragDropOperation* DragOperation = NewObject<UDragDropOperation>();
		DragOperation->Payload = this;
		
		UInventoryDummySlotWidget* DummySlotWidgetInstance = CreateWidget<UInventoryDummySlotWidget>(GetWorld(), DummySlotWidgetClass);
		const FInventoryItem* Item = InventorySystemComponent->GetItemDataByIndex(SlotIndex);
		if (!Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("Slot %d Item is invalid"), SlotIndex);
			return;
		}
		
		DummySlotWidgetInstance->SetDummy(Item->Icon, Item->Count);
	
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

	const UInventorySlotWidget* FromSlot = Cast<UInventorySlotWidget>(InOperation->Payload);
	if (IsValid(FromSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("FromSlot: %d, SlotIndex: %d"), FromSlot->SlotIndex, SlotIndex);
		
		if (FromSlot->SlotIndex != SlotIndex)
		{
			const FInventoryItem* FromItem = InventorySystemComponent->GetItemDataByIndex(FromSlot->SlotIndex);
			const FInventoryItem* ToItem = InventorySystemComponent->GetItemDataByIndex(SlotIndex);
			if (FromItem && ToItem)
			{
				// Same Item
				if (FromItem->RowName == ToItem->RowName)
				{
					InventorySystemComponent->MergeItems(FromSlot->SlotIndex, SlotIndex);
				}
				else
				{
					InventorySystemComponent->SwapItems(FromSlot->SlotIndex, SlotIndex);
				}
				
				return true;
			}
		}
	}

	return false;
}

void UInventorySlotWidget::SetItem(UTexture2D* ItemIcon, const int32 ItemCount) const
{
	// Empty Slot
	if (ItemCount == 0)
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		CountTextBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Visible);
		CountTextBorder->SetVisibility(ESlateVisibility::Visible);
		IconImage->SetBrushFromTexture(ItemIcon);
		CountTextBlock->SetText(FText::AsNumber(ItemCount));
	}
}
