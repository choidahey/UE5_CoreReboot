#include "StorageInventoryWidget.h"

#include "CR4S.h"
#include "Components/PanelWidget.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/UI/ItemSlotWidget/BaseItemSlotWidget.h"

void UStorageInventoryWidget::InitWidget(ASurvivalHUD* SurvivalHUD, const bool bNewCanSort)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(ItemSlotWidgetClass)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(ItemSlotWidgetContainer)))
	{
		return;
	}

	ItemSlotWidgetContainer->ClearChildren();
	ItemSlotWidgets.Empty();

	for (int32 Index = 0; Index < MaxSlotCount; Index++)
	{
		UBaseItemSlotWidget* ItemSlotWidget = CreateWidget<UBaseItemSlotWidget>(this, ItemSlotWidgetClass);
		if (CR4S_VALIDATE(LogInventoryUI, IsValid(ItemSlotWidget)))
		{
			ItemSlotWidget->InitSlotWidget(Index);
			ItemSlotWidgetContainer->AddChild(ItemSlotWidget);
			ItemSlotWidgets.Add(ItemSlotWidget);
		}
	}
}

void UStorageInventoryWidget::ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent)
{
	Super::ConnectInventoryComponent(NewInventoryComponent);

	SetVisibleItemSlots();
}

void UStorageInventoryWidget::SetVisibleItemSlots()
{
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	const int32 VisibleCount = InventoryComponent->GetMaxInventorySize();
	
	for (int32 Index = 0; Index < ItemSlotWidgets.Num(); Index++)
	{
		UBaseItemSlotWidget* ItemSlotWidget = ItemSlotWidgets[Index];
		if (!IsValid(ItemSlotWidget))
		{
			continue;
		}
		
		if (Index < VisibleCount)
		{
			ItemSlotWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemSlotWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
