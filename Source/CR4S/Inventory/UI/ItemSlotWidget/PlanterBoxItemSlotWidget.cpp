#include "PlanterBoxItemSlotWidget.h"

#include "CR4S.h"
#include "Blueprint/DragDropOperation.h"
#include "Inventory/Components/PlanterBoxInventoryComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

bool UPlanterBoxItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                             UDragDropOperation* InOperation)
{
	// CR4S_Log(LogInventoryUI, Warning, TEXT("NativeOnDrop"));

	UPlanterBoxInventoryComponent* PlanterBoxInventoryComponent = Cast<UPlanterBoxInventoryComponent>(
		InventoryComponent);
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(PlanterBoxInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, bCanDrop) ||
		IsValid(CurrentItem) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InOperation)))
	{
		return false;
	}

	const UBaseItemSlotWidget* FromSlot = Cast<UBaseItemSlotWidget>(InOperation->Payload);
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(FromSlot)))
	{
		return false;
	}

	UBaseInventoryItem* FromItem = FromSlot->GetCurrentItem();
	if (!CR4S_VALIDATE(LogInventoryUI, FromItem) ||
		!CR4S_VALIDATE(LogInventoryUI, IsItemAllowedByFilter(FromItem)))
	{
		return false;
	}

	const FAddItemResult Result = PlanterBoxInventoryComponent->AddItem(FromItem->GetInventoryItemData()->RowName,
	                                                                    FromItem->GetCurrentStackCount());

	FromItem->EndPassiveEffect();
	
	if (Result.AddedCount > 0)
	{
		FromSlot->GetInventoryComponent()->RemoveItemByIndex(FromSlot->GetSlotIndex(), 1);
	}

	return true;
}
