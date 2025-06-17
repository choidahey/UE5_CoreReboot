#include "ConsumableInventoryItem.h"

#include "CR4S.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Game/System/WorldTimeManager.h"
#include "Inventory/Components/BaseInventoryComponent.h"

UConsumableInventoryItem::UConsumableInventoryItem()
	: Freshness(1.f),
	  DecayRateMultiplier(1.f)
{
	bUsePassiveEffect = true;
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

void UConsumableInventoryItem::HandlePassiveEffect(const int64 NewPlayTime)
{
	Super::HandlePassiveEffect(NewPlayTime);

	EndPassiveEffect();

	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(InventoryComponent)))
	{
		return;
	}
}

void UConsumableInventoryItem::ApplyResistanceEffect()
{
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(WorldTimeManager)) ||
		!CR4S_VALIDATE(LogInventoryItem, IsValid(PlayerStatusComponent)))
	{
		return;
	}

	ClearAllEffects();

	AddResistanceBuff(EResistanceBuffType::Heat,
	                  ConsumableItemData.HeatResistanceValue,
	                  ConsumableItemData.HeatResistanceDuration);

	AddResistanceBuff(EResistanceBuffType::Humidity,
	                  ConsumableItemData.HumidityResistanceValue,
	                  ConsumableItemData.HumidityResistanceDuration);

	AddResistanceBuff(EResistanceBuffType::Cold,
	                  ConsumableItemData.ColdResistanceValue,
	                  ConsumableItemData.ColdResistanceDuration);

	if (ResistanceEffects.Num() > 0 &&
		!WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(this, &ThisClass::UpdateAllResistanceEffects))
	{
		WorldTimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::UpdateAllResistanceEffects);
	}
}

void UConsumableInventoryItem::AddResistanceBuff(const EResistanceBuffType Type,
                                                 const int32 Value,
                                                 const float DurationMinutes)
{
	if (Value <= 0 || DurationMinutes <= 0.f)
	{
		return;
	}

	auto& [Duration, Elapsed, ResistanceValue, PrevPlayTime] = ResistanceEffects.Add(Type);
	ResistanceValue = Value;
	Duration = DurationMinutes * 60.f;
	Elapsed = 0.f;
	PrevPlayTime = -1;

	ApplyThreshold(Type, Value);
	CR4S_Log(LogTemp, Warning, TEXT("Applied %s buff +%d"), *UEnum::GetValueAsString(Type), Value);
}

void UConsumableInventoryItem::UpdateAllResistanceEffects(const int64 NewPlayTime)
{
	TArray<EResistanceBuffType> ToRemove;

	for (auto& Pair : ResistanceEffects)
	{
		EResistanceBuffType Type = Pair.Key;
		FResistanceEffect& E = Pair.Value;

		if (E.PrevPlayTime < 0)
		{
			E.PrevPlayTime = NewPlayTime;
			continue;
		}

		const int64 DeltaInt = NewPlayTime - E.PrevPlayTime;
		E.PrevPlayTime = NewPlayTime;
		if (DeltaInt <= 0) continue;

		const float DeltaSec = static_cast<float>(DeltaInt);
		E.Elapsed += DeltaSec;

		if (E.Elapsed >= E.Duration)
		{
			ToRemove.Add(Type);
		}
	}

	for (const EResistanceBuffType Type : ToRemove)
	{
		ClearResistanceEffect(Type);
		ResistanceEffects.Remove(Type);
	}

	if (ResistanceEffects.Num() == 0)
	{
		WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::UpdateAllResistanceEffects);
	}
}

void UConsumableInventoryItem::ClearAllEffects()
{
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(PlayerStatusComponent)) ||
		!CR4S_VALIDATE(LogInventoryItem, IsValid(WorldTimeManager)))
	{
		return;
	}

	for (const auto& [Type, Value] : ResistanceEffects)
	{
		ApplyThreshold(Type, -Value.ResistanceValue);
		UE_LOG(LogTemp, Log, TEXT("Cleared %s buff (bulk) -%d"), *UEnum::GetValueAsString(Type), Value.ResistanceValue);
	}

	ResistanceEffects.Empty();

	WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::UpdateAllResistanceEffects);
}

void UConsumableInventoryItem::ClearResistanceEffect(const EResistanceBuffType Type)
{
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(PlayerStatusComponent)))
	{
		return;
	}

	if (const FResistanceEffect* ResistanceEffect = ResistanceEffects.Find(Type))
	{
		ApplyThreshold(Type, -ResistanceEffect->ResistanceValue);
		CR4S_Log(LogTemp, Warning, TEXT("Cleared %s buff -%d"),
		         *UEnum::GetValueAsString(Type),
		         ResistanceEffect->ResistanceValue);
	}
}

void UConsumableInventoryItem::ApplyThreshold(const EResistanceBuffType Type, const int32 Value) const
{
	switch (Type)
	{
	case EResistanceBuffType::Heat:
		PlayerStatusComponent->AddHeatThreshold(Value);
		break;
	case EResistanceBuffType::Humidity:
		PlayerStatusComponent->AddHumidityThreshold(Value);
		break;
	case EResistanceBuffType::Cold:
		PlayerStatusComponent->AddColdThreshold(Value);
		break;
	}
}
