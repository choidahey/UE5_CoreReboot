#include "PlanterBoxInventoryComponent.h"

#include "CR4S.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

UPlanterBoxInventoryComponent::UPlanterBoxInventoryComponent()
{
	MaxInventorySize = 1;
}

FAddItemResult UPlanterBoxInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	FAddItemResult Result;
	Result.RemainingCount = Count;

	const FItemInfoData* ItemData = ItemGimmickSubsystem->FindItemInfoData(RowName);
	if (!CR4S_VALIDATE(LogInventory, ItemData),
		!CR4S_VALIDATE(LogInventory, IsItemAllowedByFilter(ItemData->ItemTags)))
	{
		return Result;
	}

	Result.bSuccess = true;

	if (!HasCrops())
	{
		InventoryItems[0] = NewObject<UBaseInventoryItem>();
		if (IsValid(InventoryItems[0]))
		{
			InventoryItems[0]->InitInventoryItem(this, FInventoryItemData(RowName, *ItemData), 1);
			Result.AddedCount = 1;
			Result.RemainingCount -= 1;
		}
	}

	NotifyInventoryItemChanged(0);

	return Result;
}

bool UPlanterBoxInventoryComponent::HasCrops() const
{
	if (!CR4S_VALIDATE(LogInventory, InventoryItems.Num() > 0))
	{
		return false;
	}

	return IsValid(InventoryItems[0]);
}
