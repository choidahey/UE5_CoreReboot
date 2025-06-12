#include "BaseItemSlotWidget.h"

#include "CR4S.h"
#include "DummyItemSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/InventoryItem/ToolInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "Inventory/UI/ItemTooltipWidget.h"
#include "Inventory/UI/InventoryWidget/BaseInventoryWidget.h"

const TArray<FKey> UBaseItemSlotWidget::QuickSlotKeys = {
	EKeys::One,
	EKeys::Two,
	EKeys::Three,
	EKeys::Four,
	EKeys::Five,
	EKeys::Six,
	EKeys::Seven,
	EKeys::Eight,
	EKeys::Nine,
	EKeys::Zero,
};

UBaseItemSlotWidget::UBaseItemSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  SlotIndex(0),
	  bIsPlayerItemSlot(false),
	  bCanDrag(true),
	  bCanDrop(true),
	  bCanRemoveItem(true),
	  bCanMoveItem(true),
	  LongPressThreshold(0.5f),
	  PressStartTime(0.0),
	  bLongPressTriggered(false),
	  bCanUseItemTooltip(true)
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

void UBaseItemSlotWidget::InitSlotWidget(const int32 NewSlotIndex)
{
	SlotIndex = NewSlotIndex;

	if (IsValid(RootWidget))
	{
		RootWidget->ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::ShowToolTip);
	}
}

void UBaseItemSlotWidget::InitSlotWidgetData(const UBaseInventoryWidget* NewInventoryWidget,
                                             UBaseInventoryItem* NewItem)
{
	if (IsValid(NewInventoryWidget))
	{
		bCanDrag = NewInventoryWidget->CanDrag();
		bCanDrop = NewInventoryWidget->CanDrop();
		bCanRemoveItem = NewInventoryWidget->CanRemoveItem();
		bCanMoveItem = NewInventoryWidget->CanMoveItem();
	}

	InventoryComponent = NewInventoryWidget->GetInventoryComponent();

	SetItem(NewItem);
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
		IconImage->SetBrushFromTexture(CurrentItem->GetInventoryItemData()->ItemInfoData.Icon, true);

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

	UpdateToolTip();
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
	if (!IsValid(CurrentItem))
	{
		return FReply::Unhandled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bLongPressTriggered = false;

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				LongPressTimerHandle,
				this,
				&ThisClass::OnLongPressDetected,
				LongPressThreshold,
				false
			);
		}

		PressStartTime = FPlatformTime::Seconds();

		return FReply::Handled().PreventThrottling();
	}


	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UBaseItemSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsValid(CurrentItem))
	{
		return FReply::Unhandled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(LongPressTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(LongPressTimerHandle);
			OnShortClick();
		}
		else if (!bLongPressTriggered)
		{
			const double Duration = FPlatformTime::Seconds() - PressStartTime;
			if (Duration >= LongPressThreshold)
			{
				bLongPressTriggered = true;
				OnLongPress();
			}
			else
			{
				OnShortClick();
			}
		}

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
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
	if (!IsValid(CurrentItem) ||
		!IsValid(InventoryComponent) ||
		!IsValid(InventoryContainerWidget))
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	if (InKeyEvent.GetKey() == EKeys::G && bCanRemoveItem)
	{
		InventoryComponent->RemoveItemByIndex(SlotIndex, -1);
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::E &&
		!bIsPlayerItemSlot &&
		CR4S_VALIDATE(LogInventoryUI, bCanDrag) &&
		CR4S_VALIDATE(LogInventoryUI, bCanMoveItem))
	{
		InventoryContainerWidget->MoveItemToInventory(this, true);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBaseItemSlotWidget::UnEquipItem(const UBaseInventoryComponent* QuickSlotInventoryComponent, const int32 Index)
{
	if (CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotInventoryComponent)))
	{
		const UToolInventoryItem* ToolItem
			= Cast<UToolInventoryItem>(QuickSlotInventoryComponent->GetInventoryItemByIndex(Index));
		if (IsValid(ToolItem))
		{
			ToolItem->UnEquipItem();
		}
	}
}

void UBaseItemSlotWidget::OnShortClick()
{
	CR4S_Log(LogInventoryUI, Warning, TEXT("ShortClick!"));
}

void UBaseItemSlotWidget::OnLongPressDetected()
{
	bLongPressTriggered = true;
	OnLongPress();
}

void UBaseItemSlotWidget::OnLongPress()
{
	CR4S_Log(LogInventoryUI, Warning, TEXT("LongPressed!!!"));
}

UWidget* UBaseItemSlotWidget::ShowToolTip()
{
	if (!bCanUseItemTooltip ||
		!CR4S_VALIDATE(LogInventoryUI, ItemTooltipWidgetClass) ||
		!IsValid(CurrentItem))
	{
		return nullptr;
	}

	if (!IsValid(ItemTooltipWidget))
	{
		ItemTooltipWidget = CreateWidget<UItemTooltipWidget>(this, ItemTooltipWidgetClass);
	}

	ItemTooltipWidget->InitWidget(CurrentItem->GetInventoryItemData()->ItemInfoData);

	return ItemTooltipWidget;
}

void UBaseItemSlotWidget::CloseToolTip()
{
	SetToolTip(nullptr);

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().CloseToolTip();
	}
}

void UBaseItemSlotWidget::UpdateToolTip()
{
	CloseToolTip();

	if (IsValid(RootWidget))
	{
		SetToolTip(RootWidget->ToolTipWidgetDelegate.Execute());
	}
}
