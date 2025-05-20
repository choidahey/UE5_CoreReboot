#include "InventoryWidget.h"

#include "Inventory/InventorySystemComponent.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Inventory/UI/InventorySlotWidget.h"

void UInventoryWidget::InitInventoryWidget(UInventorySystemComponent* InventorySystemComponent,
                                           const int32 InventorySize)
{
	if (IsValid(InventorySystemComponent))
	{
		InventorySystemComponent->OnItemSlotChanged.AddDynamic(this, &ThisClass::SetItemWidget);
		SortButton->OnClicked.AddDynamic(InventorySystemComponent, &UInventorySystemComponent::SortInventoryItems);
		CloseButton->OnClicked.AddDynamic(InventorySystemComponent, &UInventorySystemComponent::CloseInventory);
	}

	InventoryWrapBox->ClearChildren();

	for (int32 Index = 0; Index < InventorySize; Index++)
	{
		UInventorySlotWidget* InventorySlotWidget = CreateWidget<UInventorySlotWidget>(
			GetWorld(), InventorySlotWidgetClass);
		InventorySlotWidget->InitWidget(InventorySystemComponent, Index);
		InventoryWrapBox->AddChild(InventorySlotWidget);
		InventorySlotWidgets.Add(InventorySlotWidget);
	}
}

void UInventoryWidget::SetItemWidget(const int32 SlotIndex, const FName& RowName, UTexture2D* ItemIcon,
                                     const int32 ItemCount)
{
	if (InventorySlotWidgets.IsValidIndex(SlotIndex))
	{
		InventorySlotWidgets[SlotIndex]->SetItem(ItemIcon, ItemCount);
		UE_LOG(LogTemp, Warning, TEXT("SlotIndex: %d / ItemCount: %d"), SlotIndex, ItemCount);
	}
}
