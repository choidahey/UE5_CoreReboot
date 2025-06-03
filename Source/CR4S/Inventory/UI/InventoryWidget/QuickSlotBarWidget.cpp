#include "QuickSlotBarWidget.h"

#include "Inventory/UI/ItemSlotWidget/BaseItemSlotWidget.h"
#include "CR4S.h"
#include "Components/HorizontalBox.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void UQuickSlotBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotBar)))
	{
		return;
	}
	
	for (UWidget* ChildWidget : QuickSlotBar->GetAllChildren())
	{
		UBaseItemSlotWidget* ItemSlotWidget = Cast<UBaseItemSlotWidget>(ChildWidget);
		if (IsValid(ItemSlotWidget))
		{
			QuickSlotWidgets.AddUnique(ItemSlotWidget);
		}
	}
}

void UQuickSlotBarWidget::InitWidget(UPlayerInventoryComponent* InPlayerInventoryComponent)
{
	PlayerInventoryComponent = InPlayerInventoryComponent;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotBar)))
	{
		return;
	}

	PlayerInventoryComponent->OnQuickSlotItemChanged.AddUniqueDynamic(this, &ThisClass::UpdateQuickSlotWidget);

	int32 SlotIndex = 0;
	for (UWidget* ChildWidget : QuickSlotBar->GetAllChildren())
	{
		UBaseItemSlotWidget* SlotWidget = Cast<UBaseItemSlotWidget>(ChildWidget);
		if (IsValid(SlotWidget))
		{
			UBaseInventoryItem* Item = PlayerInventoryComponent->GetQuickSlotItemDataByIndex(SlotIndex);
			SlotWidget->InitWidget(PlayerInventoryComponent, Item, true, true);
			SlotWidget->SetCanMoveItem(false);
			QuickSlotWidgets.AddUnique(SlotWidget);
			SlotIndex++;
		}
	}
}

void UQuickSlotBarWidget::UpdateQuickSlotWidget(UBaseInventoryItem* Item)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(Item)))
	{
		return;
	}

	const int32 SlotIndex = Item->GetSlotIndex();
	if (QuickSlotWidgets.IsValidIndex(SlotIndex))
	{
		QuickSlotWidgets[SlotIndex]->SetItem(Item);
	}
}
