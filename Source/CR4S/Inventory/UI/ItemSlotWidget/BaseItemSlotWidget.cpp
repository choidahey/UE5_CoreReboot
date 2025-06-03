#include "BaseItemSlotWidget.h"

#include "CR4S.h"
#include "DummyItemSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "Inventory/UI/InventoryWidget/BaseInventoryWidget.h"
#include "Inventory/UI/InventoryWidget/PlayerInventoryWidget.h"

void UBaseItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HoverImage))
	{
		HoverImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	PlayerController = GetOwningPlayer();

	InventoryContainerWidget = GetTypedOuter<UInventoryContainerWidget>();

	bCanMoveItem = true;

	SetIsFocusable(true);
}

void UBaseItemSlotWidget::InitWidget(UBaseInventoryItem* NewItem, const bool bNewCanDrag, const bool bNewCanDrop)
{
	const UBaseInventoryWidget* InventoryWidget = GetTypedOuter<UBaseInventoryWidget>();
	if (IsValid(InventoryWidget))
	{
		bIsPlayerItemSlot = InventoryWidget->IsA(UPlayerInventoryWidget::StaticClass());
		InventoryComponent = InventoryWidget->GetInventoryComponent();
	}

	CurrentItem = NewItem;

	SetItem(CurrentItem);

	bCanDrag = bNewCanDrag;
	bCanDrop = bNewCanDrop;
}

void UBaseItemSlotWidget::SetItem(UBaseInventoryItem* InItem)
{
	CurrentItem = InItem;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(IconImage)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CountTextBlock)))
	{
		return;
	}

	if (CurrentItem->HasItemData())
	{
		IconImage->SetVisibility(ESlateVisibility::Visible);
		IconImage->SetBrushFromTexture(CurrentItem->GetInventoryItemData()->ItemInfoData.Icon);

		if (CurrentItem->GetInventoryItemData()->ItemInfoData.MaxStackCount > 1)
		{
			CountTextBlock->SetVisibility(ESlateVisibility::Visible);
			CountTextBlock->SetText(FText::AsNumber(CurrentItem->GetCurrentStackCount()));
		}
		else
		{
			CountTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (IsValid(InventoryComponent))
		{
			InventoryComponent->AddOccupiedSlot(CurrentItem->GetSlotIndex());
		}
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		CountTextBlock->SetVisibility(ESlateVisibility::Collapsed);

		if (IsValid(InventoryComponent))
		{
			InventoryComponent->RemoveOccupiedSlot(CurrentItem->GetSlotIndex());
		}
	}
}

void UBaseItemSlotWidget::EmptyItem()
{
	CurrentItem = nullptr;
}

void UBaseItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (IsValid(HoverImage))
	{
		HoverImage->SetVisibility(ESlateVisibility::Visible);
	}

	if (CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerController)))
	{
		PlayerController->SetInputMode(FInputModeUIOnly()
		                               .SetWidgetToFocus(this->TakeWidget())
		                               .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	}
}

void UBaseItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (IsValid(HoverImage))
	{
		HoverImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerController)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryContainerWidget)) ||
		!InventoryContainerWidget->IsOpen())
	{
		return;
	}

	PlayerController->SetInputMode(FInputModeUIOnly()
	                               .SetWidgetToFocus(InventoryContainerWidget->TakeWidget())
	                               .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
}

bool UBaseItemSlotWidget::IsItemAllowedByFilter(const UBaseInventoryItem* Item) const
{
	if (!IsValid(InventoryComponent) || !IsValid(Item))
	{
		return false;
	}

	return InventoryComponent->IsItemAllowedByFilter(Item->GetInventoryItemData()->ItemInfoData.ItemTags);
}

FReply UBaseItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, CurrentItem->HasItemData()))
	{
		return FReply::Unhandled();
	}

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

	if (!CR4S_VALIDATE(LogInventoryUI, bCanDrag) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(DummySlotWidgetClass)))
	{
		return;
	}

	UDragDropOperation* DragOperation = NewObject<UDragDropOperation>();
	DragOperation->Payload = this;

	UDummyItemSlotWidget* DummySlotWidgetInstance = CreateWidget<UDummyItemSlotWidget>(
		GetWorld(), DummySlotWidgetClass);

	DummySlotWidgetInstance->SetDummy(CurrentItem->GetInventoryItemData()->ItemInfoData.Icon,
	                                  CurrentItem->GetCurrentStackCount());

	DragOperation->DefaultDragVisual = DummySlotWidgetInstance;

	OutOperation = DragOperation;
}

bool UBaseItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                       UDragDropOperation* InOperation)
{
	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnDrop"));

	if (!CR4S_VALIDATE(LogInventoryUI, bCanDrop) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)) ||
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
	if (!CR4S_VALIDATE(LogInventoryUI, FromItem) ||
		!CR4S_VALIDATE(LogInventoryUI, IsItemAllowedByFilter(FromItem)) ||
		!CR4S_VALIDATE(LogInventoryUI, FromSlot->IsItemAllowedByFilter(CurrentItem)))
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

FReply UBaseItemSlotWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!IsValid(CurrentItem) || !CurrentItem->HasItemData())
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	if (InKeyEvent.GetKey() == EKeys::G)
	{
		CurrentItem->SetCurrentStackCount(0);
		SetItem(CurrentItem);
		return FReply::Handled();
	}

	const bool bPressedE = InKeyEvent.GetKey() == EKeys::E;
	const bool bPressedQ = InKeyEvent.GetKey() == EKeys::Q;

	const bool bCanMoveToPlayer = bPressedE && !bIsPlayerItemSlot;
	const bool bCanMoveToOther = bPressedQ && bIsPlayerItemSlot;

	if ((bCanMoveToPlayer || bCanMoveToOther) &&
		CR4S_VALIDATE(LogInventoryUI, bCanDrag) &&
		CR4S_VALIDATE(LogInventoryUI, bCanMoveItem) &&
		IsValid(InventoryContainerWidget))
	{
		InventoryContainerWidget->MoveItemToInventory(this, bCanMoveToPlayer);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
