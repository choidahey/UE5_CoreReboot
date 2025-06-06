#include "PlayerItemSlotWidget.h"

#include "CR4S.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"

UPlayerItemSlotWidget::UPlayerItemSlotWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPlayerItemSlot = true;
}

void UPlayerItemSlotWidget::InitSlotWidgetData(const UBaseInventoryWidget* NewInventoryWidget,
                                               UBaseInventoryItem* NewItem)
{
	Super::InitSlotWidgetData(NewInventoryWidget, NewItem);

	PlayerInventoryComponent = Cast<UPlayerInventoryComponent>(InventoryComponent);
}

FReply UPlayerItemSlotWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!IsValid(CurrentItem) ||
		!IsValid(InventoryComponent) ||
		!IsValid(InventoryContainerWidget))
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}
	
	if (InKeyEvent.GetKey() == EKeys::Q)
	{
		InventoryContainerWidget->MoveItemToInventory(this, false);
		return FReply::Handled();
	}

	const int32 QuickSlotIndex = QuickSlotKeys.IndexOfByKey(InKeyEvent.GetKey());
	if (QuickSlotIndex != INDEX_NONE)
	{
		MoveItemToQuickSlot(QuickSlotIndex);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPlayerItemSlotWidget::MoveItemToQuickSlot(const int32 Index) const
{
	if (CR4S_VALIDATE(LogInventoryUI, PlayerInventoryComponent))
	{
		UBaseInventoryComponent* QuickSlotInventoryComponent = PlayerInventoryComponent->
			GetQuickSlotInventoryComponent();
		if (CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotInventoryComponent)))
		{
			UnEquipItem(QuickSlotInventoryComponent, Index);
			QuickSlotInventoryComponent->SwapItem(InventoryComponent, SlotIndex, Index);
		}
	}
}
