#include "InventoryWidget.h"

#include "CR4S.h"
#include "Inventory/InventoryComponent.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Inventory/UI/InventorySlotWidget.h"

void UInventoryWidget::InitInventoryWidget(UInventoryComponent* InventorySystemComponent)
{
	if (CR4S_VALIDATE(LogInventoryUI, IsValid(InventorySystemComponent)))
	{
		InventorySystemComponent->OnItemSlotChanged.AddUniqueDynamic(this, &ThisClass::SetItemWidget);
		SortButton->OnClicked.AddUniqueDynamic(InventorySystemComponent, &UInventoryComponent::SortInventoryItems);
		CloseButton->OnClicked.AddUniqueDynamic(InventorySystemComponent, &UInventoryComponent::CloseInventory);
	}

	int32 Index = 0;
	for (UWidget* ChildWidget : InventoryBox->GetAllChildren())
	{
		const UHorizontalBox* HorizontalBox = Cast<UHorizontalBox>(ChildWidget);
		if (IsValid(HorizontalBox))
		{
			for (UWidget* ChildWidget2 : HorizontalBox->GetAllChildren())
			{
				UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(ChildWidget2);
				if (IsValid(SlotWidget))
				{
					UBaseInventoryItem* Item = InventorySystemComponent->GetItemDataByIndex(Index);
					SlotWidget->InitWidget(InventorySystemComponent, Item);
					SetItemWidget(Item);
					
					InventorySlotWidgets.AddUnique(SlotWidget);

					Index++;
				}
			}
		}
	}
}

void UInventoryWidget::SetItemWidget(UBaseInventoryItem* Item)
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
