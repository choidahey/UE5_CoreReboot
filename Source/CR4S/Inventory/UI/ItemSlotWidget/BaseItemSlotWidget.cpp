#include "BaseItemSlotWidget.h"

#include "CR4S.h"
#include "DummyItemSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"

UBaseItemSlotWidget::UBaseItemSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  SlotIndex(0),
	  bIsPlayerItemSlot(false),
	  bCanDrag(true),
	  bCanDrop(true),
	  bCanRemoveItem(true),
	  bCanMoveItem(true)
{
}

void UBaseItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HoverImage))
	{
		HoverImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	PlayerController = GetOwningPlayer();

	InventoryContainerWidget = GetTypedOuter<UInventoryContainerWidget>();

	SetIsFocusable(true);
}

void UBaseItemSlotWidget::InitSlotWidget(int32 NewSlotIndex)
{
	SlotIndex = NewSlotIndex;
}

void UBaseItemSlotWidget::InitSlotWidgetData(UBaseInventoryComponent* NewInventoryComponent,
                                             UBaseInventoryItem* NewItem, const bool bNewCanDrag,
                                             const bool bNewCanDrop)
{
	if (IsValid(NewInventoryComponent))
	{
		bIsPlayerItemSlot = NewInventoryComponent->IsA(UPlayerInventoryComponent::StaticClass());
	}

	InventoryComponent = NewInventoryComponent;

	SetItem(NewItem);

	bCanDrag = bNewCanDrag;
	bCanDrop = bNewCanDrop;
}

void UBaseItemSlotWidget::SetItem(UBaseInventoryItem* InItem)
{
	CurrentItem = InItem;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(IconImage)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CountTextBlock)))
	{
		return;
	}

	if (IsValid(CurrentItem))
	{
		IconImage->SetVisibility(ESlateVisibility::Visible);
		IconImage->SetBrushFromTexture(CurrentItem->GetInventoryItemData()->ItemInfoData.Icon);

		if (CurrentItem->IsStackableItem())
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
			InventoryComponent->AddOccupiedSlot(SlotIndex);
		}
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		CountTextBlock->SetVisibility(ESlateVisibility::Collapsed);

		if (IsValid(InventoryComponent))
		{
			InventoryComponent->RemoveOccupiedSlot(SlotIndex);
		}
	}
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
	CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent));

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CurrentItem)))
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
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InOperation)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent)))
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
		!CR4S_VALIDATE(LogInventoryUI, IsItemAllowedByFilter(FromItem)))
	{
		return false;
	}

	if (IsValid(CurrentItem))
	{
		if (!FromSlot->IsItemAllowedByFilter(CurrentItem))
		{
			return false;
		}

		if (FromItem != CurrentItem)
		{
			// Same Item
			if (FromItem->GetInventoryItemData()->RowName == CurrentItem->GetInventoryItemData()->RowName)
			{
				InventoryComponent->MergeItem(FromSlot->InventoryComponent, FromSlot->SlotIndex, SlotIndex);
			}
			else
			{
				InventoryComponent->SwapItem(FromSlot->InventoryComponent, FromSlot->SlotIndex, SlotIndex);
			}

			return true;
		}
	}
	else
	{
		InventoryComponent->SwapItem(FromSlot->InventoryComponent, FromSlot->SlotIndex, SlotIndex);
	}

	return false;
}

FReply UBaseItemSlotWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!IsValid(CurrentItem) || !IsValid(InventoryComponent))
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	if (InKeyEvent.GetKey() == EKeys::G && bCanRemoveItem)
	{
		InventoryComponent->RemoveItemByIndex(SlotIndex, -1);
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
