#include "InventoryFilterData.h"

#include "CR4S.h"

bool UInventoryFilterData::IsAllowedItem(const FGameplayTagContainer& ItemTags) const
{
	if (!AllowedTagQuery.IsEmpty())
	{
		if (!AllowedTagQuery.Matches(ItemTags))
		{
			CR4S_Log(LogInventory, Warning, TEXT("%s is denied"), *ItemTags.ToString());
			return false;
		}
	}

	if (!DeniedTagQuery.IsEmpty())
	{
		if (DeniedTagQuery.Matches(ItemTags))
		{
			CR4S_Log(LogInventory, Warning, TEXT("%s is denied"), *ItemTags.ToString());
			return false;
		}
	}

	return true;
}
