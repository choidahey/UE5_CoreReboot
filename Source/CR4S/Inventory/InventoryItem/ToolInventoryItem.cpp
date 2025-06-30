#include "ToolInventoryItem.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Kismet/GameplayStatics.h"

UToolInventoryItem::UToolInventoryItem()
{
	ToolItemData = FToolItemData();

	DefaultTag = FGameplayTag::RequestGameplayTag(FName("Als.OverlayMode.Default"));
}

void UToolInventoryItem::InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
                                           const FInventoryItemData& NewInventoryItemData, const int32 StackCount)
{
	Super::InitInventoryItem(NewInventoryComponent, NewInventoryItemData, StackCount);

	const AActor* PlayerCharacter = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	if (IsValid(PlayerCharacter))
	{
		PlayerInventoryComponent = PlayerCharacter->FindComponentByClass<UPlayerInventoryComponent>();
	}
	
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
	}
}
