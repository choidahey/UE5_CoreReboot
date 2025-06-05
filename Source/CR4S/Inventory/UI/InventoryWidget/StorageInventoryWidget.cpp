#include "StorageInventoryWidget.h"

#include "CR4S.h"
#include "Components/PanelWidget.h"
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
