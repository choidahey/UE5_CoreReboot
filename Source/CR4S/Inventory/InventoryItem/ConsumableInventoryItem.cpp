#include "ConsumableInventoryItem.h"

#include "CR4S.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Game/System/WorldTimeManager.h"
#include "Inventory/Components/BaseInventoryComponent.h"

UConsumableInventoryItem::UConsumableInventoryItem()
	: HeatEffectDuration(0),
	  HumidityEffectDuration(0),
	  ColdEffectDuration(0)
{
}

void UConsumableInventoryItem::InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
                                                 const FInventoryItemData& NewInventoryItemData, const int32 StackCount)
{
	Super::InitInventoryItem(NewInventoryComponent, NewInventoryItemData, StackCount);

	const UDataTable* DataTable = NewInventoryItemData.ItemInfoData.DetailData.DataTable;
	if (!CR4S_VALIDATE(LogInventory, IsValid(DataTable)))
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(World)))
	{
		return;
	}

	WorldTimeManager = World->GetSubsystem<UWorldTimeManager>();

	if (const FConsumableItemData* FindItemData =
		DataTable->FindRow<FConsumableItemData>(NewInventoryItemData.RowName, TEXT("")))
	{
		ConsumableItemData = *FindItemData;
	}
}

void UConsumableInventoryItem::UseItem(const int32 Index)
{
	Super::UseItem(Index);

	if (!CR4S_VALIDATE(LogInventory, IsValid(InventoryComponent)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(InventoryComponent->GetOwner())))
	{
		return;
	}

	if (IsValid(PlayerStatusComponent))
	{
		PlayerStatusComponent->AddCurrentHunger(ConsumableItemData.HungerRestore);
		PlayerStatusComponent->AddCurrentHP(ConsumableItemData.HealthRestore);
		
		ApplyResistanceEffect();

		InventoryComponent->RemoveItemByIndex(Index, 1);
	}
}

void UConsumableInventoryItem::HandlePassiveEffect()
{
	Super::HandlePassiveEffect();

	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(InventoryComponent)))
	{
		return;
	}
}

void UConsumableInventoryItem::ApplyResistanceEffect()
{
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(WorldTimeManager)))
	{
		return;
	}

	if (ConsumableItemData.HeatResistanceValue > 0)
	{
		HeatEffectDuration = ConsumableItemData.HeatResistanceDuration * 60;

		PlayerStatusComponent->AddHeatThreshold(ConsumableItemData.HeatResistanceValue);

		WorldTimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::UpdateHeatResistanceEffect);

		CR4S_Log(LogInventoryItem, Warning, TEXT("+%d"), ConsumableItemData.HeatResistanceValue);
	}

	if (ConsumableItemData.HumidityResistanceValue > 0)
	{
		HeatEffectDuration = ConsumableItemData.HumidityResistanceDuration * 60;

		PlayerStatusComponent->AddHeatThreshold(ConsumableItemData.HumidityResistanceValue);

		WorldTimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::UpdateHumidityResistanceEffect);
		
		CR4S_Log(LogInventoryItem, Warning, TEXT("+%d"), ConsumableItemData.HumidityResistanceValue);
	}

	if (ConsumableItemData.ColdResistanceValue > 0)
	{
		HeatEffectDuration = ConsumableItemData.ColdResistanceDuration * 60;

		PlayerStatusComponent->AddHeatThreshold(ConsumableItemData.ColdResistanceValue);

		WorldTimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::UpdateColdResistanceEffect);
		
		CR4S_Log(LogInventoryItem, Warning, TEXT("+%d"), ConsumableItemData.ColdResistanceValue);
	}
}

void UConsumableInventoryItem::ClearResistanceEffect(const EResistanceBuffType Type) const
{
	switch (Type)
	{
	case EResistanceBuffType::Heat:
		PlayerStatusComponent->AddHeatThreshold(-ConsumableItemData.HeatResistanceValue);
		CR4S_Log(LogInventoryItem, Warning, TEXT("-%d"), ConsumableItemData.HeatResistanceValue);
		break;
	case EResistanceBuffType::Humidity:
		PlayerStatusComponent->AddHeatThreshold(-ConsumableItemData.HumidityResistanceValue);
		CR4S_Log(LogInventoryItem, Warning, TEXT("-%d"), ConsumableItemData.HumidityResistanceValue);
		break;
	case EResistanceBuffType::Cold:
		PlayerStatusComponent->AddHeatThreshold(-ConsumableItemData.ColdResistanceValue);
		CR4S_Log(LogInventoryItem, Warning, TEXT("-%d"), ConsumableItemData.ColdResistanceValue);
		break;
	}
}

void UConsumableInventoryItem::UpdateHeatResistanceEffect(int64 PlayTime)
{
	HeatEffectDuration--;

	if (HeatEffectDuration <= 0)
	{
		ClearResistanceEffect(EResistanceBuffType::Heat);
	}

	if (IsValid(WorldTimeManager) && WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(
		this, &ThisClass::UpdateHeatResistanceEffect))
	{
		WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::UpdateHeatResistanceEffect);
	}
}

void UConsumableInventoryItem::UpdateHumidityResistanceEffect(int64 PlayTime)
{
	HumidityEffectDuration--;

	if (HumidityEffectDuration <= 0)
	{
		ClearResistanceEffect(EResistanceBuffType::Humidity);
	}

	if (IsValid(WorldTimeManager) && WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(
		this, &ThisClass::UpdateHumidityResistanceEffect))
	{
		WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::UpdateHumidityResistanceEffect);
	}
}

void UConsumableInventoryItem::UpdateColdResistanceEffect(int64 PlayTime)
{
	ColdEffectDuration--;

	if (ColdEffectDuration <= 0)
	{
		ClearResistanceEffect(EResistanceBuffType::Cold);
	}

	if (IsValid(WorldTimeManager) && WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(
		this, &ThisClass::UpdateColdResistanceEffect))
	{
		WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::UpdateColdResistanceEffect);
	}
}
