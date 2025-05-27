#include "QuickSlotBarWidget.h"

#include "Inventory/UI/ItemSlotWidget/BaseItemSlotWidget.h"
#include "CR4S.h"
#include "Components/HorizontalBox.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

bool UQuickSlotBarWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	MinQuickSlotIndex = 40;
	MaxQuickSlotIndex = 49;

	return true;
}

void UQuickSlotBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (UWidget* ChildWidget : QuickSlotBar->GetAllChildren())
	{
		UBaseItemSlotWidget* ItemSlotWidget = Cast<UBaseItemSlotWidget>(ChildWidget);
		if (IsValid(ItemSlotWidget))
		{
			QuickSlotWidgets.AddUnique(ItemSlotWidget);
		}
	}
}

void UQuickSlotBarWidget::InitWidget(UInventoryComponent* InInventorySystemComponent)
{
	InventorySystemComponent = InInventorySystemComponent;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(InventorySystemComponent)))
	{
		return;
	}

	InventorySystemComponent->OnItemSlotChanged.AddUniqueDynamic(this, &ThisClass::SetItemWidget);

	int32 SlotIndex = MinQuickSlotIndex;
	for (int32 Index = 0; Index < QuickSlotWidgets.Num(); Index++)
	{
		UBaseItemSlotWidget* ItemSlotWidget = QuickSlotWidgets[Index];
		if (!IsValid(ItemSlotWidget))
		{
			continue;
		}

		UBaseInventoryItem* Item = InventorySystemComponent->GetItemDataByIndex(SlotIndex);
		ItemSlotWidget->InitWidget(InventorySystemComponent, Item);
		SetItemWidget(Item);

		SlotIndex++;
	}
}

void UQuickSlotBarWidget::SetItemWidget(UBaseInventoryItem* Item)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(Item)))
	{
		return;
	}

	const int32 SlotIndex = Item->GetSlotIndex() - MinQuickSlotIndex;
	if (QuickSlotWidgets.IsValidIndex(SlotIndex))
	{
		QuickSlotWidgets[SlotIndex]->SetItem(Item);
	}
}
