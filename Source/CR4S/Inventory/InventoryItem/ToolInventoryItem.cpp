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

	if (IsValid(OwnerPlayer))
	{
		StatusComponent = OwnerPlayer->FindComponentByClass<UBaseStatusComponent>();
	}
}

void UToolInventoryItem::UseItem(const int32 Index)
{
	Super::UseItem(Index);

	if (!CR4S_VALIDATE(LogInventory, IsValid(OwnerPlayer)))
	{
		return;
	}

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
	if (CR4S_VALIDATE(LogInventory, IsValid(OwnerPlayer)) &&
		IsValid(PlayerInventoryComponent))
	{
		PlayerInventoryComponent->SetHeldToolTag(ToolItemData.ToolTag);
		OwnerPlayer->SetCurrentToolByTag(ToolItemData.ToolTag);

		if (IsValid(StatusComponent))
		{
			StatusComponent->AddAttackPower(ToolItemData.Damage);
			CR4S_Log(LogInventory, Warning, TEXT("AttackPower: %.f"), StatusComponent->GetAttackPower());
		}
	}
}

void UToolInventoryItem::UnEquipItem() const
{
	if (CR4S_VALIDATE(LogInventory, IsValid(OwnerPlayer)) &&
		IsValid(PlayerInventoryComponent) &&
		PlayerInventoryComponent->GetHeldToolTag() != FGameplayTag::EmptyTag)
	{
		PlayerInventoryComponent->SetHeldToolTag(FGameplayTag::EmptyTag);
		OwnerPlayer->SetCurrentToolByTag(DefaultTag);

		if (IsValid(StatusComponent))
		{
			StatusComponent->AddAttackPower(-ToolItemData.Damage);
			CR4S_Log(LogInventory, Warning, TEXT("AttackPower: %.f"), StatusComponent->GetAttackPower());
		}
	}
}
