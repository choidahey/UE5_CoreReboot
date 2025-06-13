#include "ToolInventoryItem.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

UToolInventoryItem::UToolInventoryItem()
{
	ToolItemData = FToolItemData();

	DefaultTag = FGameplayTag::RequestGameplayTag(FName("Als.OverlayMode.Default"));
}

void UToolInventoryItem::InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
										   const FInventoryItemData& NewInventoryItemData, const int32 StackCount)
{
	Super::InitInventoryItem(NewInventoryComponent, NewInventoryItemData, StackCount);

	const UDataTable* DataTable = NewInventoryItemData.ItemInfoData.DetailData.DataTable;
	if (!CR4S_VALIDATE(LogInventory, IsValid(DataTable)))
	{
		return;
	}

	if (const FToolItemData* FindItemData =
		DataTable->FindRow<FToolItemData>(NewInventoryItemData.RowName, TEXT("")))
	{
		ToolItemData = *FindItemData;
	}
}

void UToolInventoryItem::UseItem(const int32 Index)
{
	Super::UseItem(Index);

	if (!CR4S_VALIDATE(LogInventory, IsValid(OwnerPlayer)))
	{
		return;
	}

	CR4S_Log(LogInventory, Warning, TEXT("%s"), *OwnerPlayer->GetOverlayMode().ToString());

	if (OwnerPlayer->GetOverlayMode() == ToolItemData.ToolTag)
	{
		UnEquipItem();
	}
	else
	{
		EquipItem();
	}
}

void UToolInventoryItem::EquipItem() const
{
	if (CR4S_VALIDATE(LogInventory, IsValid(OwnerPlayer)))
	{
		OwnerPlayer->SetCurrentToolByTag(ToolItemData.ToolTag);
		PlayerInventoryComponent->SetHeldToolTag(ToolItemData.ToolTag);

		if (IsValid(PlayerStatusComponent))
		{
			PlayerStatusComponent->AddAttackPower(ToolItemData.Damage);
		}
	}
}

void UToolInventoryItem::UnEquipItem() const
{
	if (CR4S_VALIDATE(LogInventory, IsValid(OwnerPlayer)))
	{
		OwnerPlayer->SetCurrentToolByTag(DefaultTag);
		PlayerInventoryComponent->SetHeldToolTag(FGameplayTag());

		if (IsValid(PlayerStatusComponent))
		{
			PlayerStatusComponent->AddAttackPower(-ToolItemData.Damage);
		}
	}
}
