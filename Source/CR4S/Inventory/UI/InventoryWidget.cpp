#include "InventoryWidget.h"

#include "CR4S.h"
#include "Inventory/InventorySystemComponent.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Inventory/UI/InventorySlotWidget.h"

void UInventoryWidget::InitInventoryWidget(UInventorySystemComponent* InventorySystemComponent)
{
	if (IsValid(InventorySystemComponent))
	{
		InventorySystemComponent->OnItemSlotChanged.AddUniqueDynamic(this, &ThisClass::SetItemWidget);
		SortButton->OnClicked.AddUniqueDynamic(InventorySystemComponent, &UInventorySystemComponent::SortInventoryItems);
		CloseButton->OnClicked.AddUniqueDynamic(InventorySystemComponent, &UInventorySystemComponent::CloseInventory);
	}

	InventoryWrapBox->ClearChildren();

	for (int32 Index = 0; Index < InventorySystemComponent->GetMaxInventorySlot(); Index++)
	{
		UInventorySlotWidget* InventorySlotWidget = CreateWidget<UInventorySlotWidget>(
			GetWorld(), InventorySlotWidgetClass);
		InventorySlotWidget->InitWidget(InventorySystemComponent, InventorySystemComponent->GetItemDataByIndex(Index));
		InventoryWrapBox->AddChild(InventorySlotWidget);
		InventorySlotWidgets.Add(InventorySlotWidget);
	}
}

void UInventoryWidget::SetItemWidget(UBaseInventoryItem* Item)
{
	if (!IsValid(Item))
	{
		CR4S_Log(LogTemp, Warning, TEXT("Item is invalid"));
		return;
	}

	const int32 SlotIndex = Item->GetSlotIndex(); 
	if (InventorySlotWidgets.IsValidIndex(SlotIndex))
	{
		InventorySlotWidgets[SlotIndex]->SetItem(Item);
		UE_LOG(LogTemp, Warning, TEXT("SlotIndex: %d / ItemCount: %d"), SlotIndex, Item->GetCurrentStackCount());
	}
}
