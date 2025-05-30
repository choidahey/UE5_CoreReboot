#include "BaseInventoryWidget.h"

#include "CR4S.h"
#include "Components/Button.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "Inventory/UI/ItemSlotWidget/BaseItemSlotWidget.h"

void UBaseInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = GetOwningPlayer();

	InventoryContainerWidget = GetTypedOuter<UInventoryContainerWidget>();

	SetIsFocusable(true);

	bCanSort = true;
}

void UBaseInventoryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerController)))
	{
		PlayerController->SetInputMode(FInputModeUIOnly()
		                               .SetWidgetToFocus(this->TakeWidget())
		                               .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
	}
}

void UBaseInventoryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

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

void UBaseInventoryWidget::InitWidget(ASurvivalHUD* SurvivalHUD, const bool bNewCanSort)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)))
	{
		return;
	}

	for (UWidget* ChildWidget : ItemSlotWidgetContainer->GetAllChildren())
	{
		UBaseItemSlotWidget* ItemSlotWidget = Cast<UBaseItemSlotWidget>(ChildWidget);
		if (IsValid(ItemSlotWidget))
		{
			ItemSlotWidgets.AddUnique(ItemSlotWidget);
		}
	}

	bCanSort = bNewCanSort;
}

void UBaseInventoryWidget::ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent,
                                                     const bool bCanDrag, const bool bCanDrop)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(NewInventoryComponent)))
	{
		return;
	}

	InventoryComponent = NewInventoryComponent;

	NewInventoryComponent->OnItemSlotChanged.AddUniqueDynamic(this, &ThisClass::UpdateItemSlotWidget);

	int32 SlotIndex = 0;
	for (UBaseItemSlotWidget* ItemSlotWidget : ItemSlotWidgets)
	{
		if (IsValid(ItemSlotWidget))
		{
			UBaseInventoryItem* Item = InventoryComponent->GetItemDataByIndex(SlotIndex);
			ItemSlotWidget->InitWidget(Item, bCanDrag, bCanDrop);
		}

		SlotIndex++;
	}
}

void UBaseInventoryWidget::UnBoundOnItemSlotChanged()
{
	if (IsValid(InventoryComponent) &&
		InventoryComponent->OnItemSlotChanged.IsAlreadyBound(this, &ThisClass::UpdateItemSlotWidget))
	{
		InventoryComponent->OnItemSlotChanged.RemoveDynamic(this, &ThisClass::UpdateItemSlotWidget);
	}
}

void UBaseInventoryWidget::UpdateItemSlotWidget(UBaseInventoryItem* Item)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(Item)))
	{
		return;
	}

	const int32 SlotIndex = Item->GetSlotIndex();
	if (ItemSlotWidgets.IsValidIndex(SlotIndex))
	{
		ItemSlotWidgets[SlotIndex]->SetItem(Item);
	}
}

FReply UBaseInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent)))
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	if (InKeyEvent.GetKey() == EKeys::R && CR4S_VALIDATE(LogInventoryUI, bCanSort))
	{
		InventoryComponent->SortInventoryItems();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
