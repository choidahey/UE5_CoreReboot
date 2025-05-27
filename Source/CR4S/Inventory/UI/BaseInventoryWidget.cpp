#include "BaseInventoryWidget.h"

#include "CR4S.h"
#include "Components/Button.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "ItemSlotWidget/InventorySlotWidget.h"

void UBaseInventoryWidget::InitInventoryWidget(UBaseInventoryComponent* InventoryComponent)
{
	if (CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent)))
	{
		InventoryComponent->OnItemSlotChanged.AddUniqueDynamic(this, &ThisClass::UpdateItemSlotWidget);
	}

	int32 SlotIndex = 0;
	for (UWidget* ChildWidget : InventoryBox->GetAllChildren())
	{
		UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(ChildWidget);
		if (IsValid(SlotWidget))
		{
			UBaseInventoryItem* Item = InventoryComponent->GetItemDataByIndex(SlotIndex);
			SlotWidget->InitWidget(InventoryComponent, Item);
			InventorySlotWidgets.AddUnique(SlotWidget);
			SlotIndex++;
		}
	}
}

void UBaseInventoryWidget::UpdateItemSlotWidget(UBaseInventoryItem* Item)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(Item)))
	{
		return;
	}
	
	const int32 SlotIndex = Item->GetSlotIndex();
	if (InventorySlotWidgets.IsValidIndex(SlotIndex))
	{
		InventorySlotWidgets[SlotIndex]->SetItem(Item);
	}
}
