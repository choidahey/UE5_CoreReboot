#include "BaseInventoryComponent.h"

#include "CR4S.h"
#include "Gimmick/GimmickObjects/ItemPouchGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryFilterData/InventoryFilterData.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

UBaseInventoryComponent::UBaseInventoryComponent()
	: MaxItemSlot(10)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<AActor>(GetOwner());
	if (!CR4S_VALIDATE(LogInventory, IsValid(OwnerActor)))
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (CR4S_VALIDATE(LogInventory, IsValid(World)))
	{
		ItemGimmickSubsystem = World->GetSubsystem<UItemGimmickSubsystem>();
	}

	InitInventorySize();
}

void UBaseInventoryComponent::InitInventorySize()
{
	InventoryItems.Reserve(MaxItemSlot);
	for (int32 Index = 0; Index < MaxItemSlot; ++Index)
	{
		UBaseInventoryItem* Item = NewObject<UBaseInventoryItem>(this);
		Item->InitInventoryItem(OwnerActor, Index);
		InventoryItems.Add(Item);
	}
}

void UBaseInventoryComponent::AddItems(const TMap<FName, int32>& Items)
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	TMap<FName, int32> RemainingItems;
	for (const auto& [RowName, Count] : Items)
	{
		const FAddItemResult Result = AddItem(RowName, Count);

		if (Result.RemainingCount > 0)
		{
			RemainingItems.FindOrAdd(RowName) += Result.RemainingCount;
		}
	}

	if (RemainingItems.Num() > 0)
	{
		const AItemPouchGimmick* ItemPouch =
			ItemGimmickSubsystem->SpawnGimmickByRowName<AItemPouchGimmick>("ItemPouch",
			                                                               OwnerActor->GetActorLocation(),
			                                                               OwnerActor->GetActorRotation());

		ItemPouch->InitItemPouch(RemainingItems);
	}
}

FAddItemResult UBaseInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	FAddItemResult Result;
	Result.RemainingCount = Count;

	if (!CR4S_VALIDATE(LogInventory, Count > 0) ||
		!CR4S_VALIDATE(LogInventory, IsValid(ItemGimmickSubsystem)))
	{
		return Result;
	}

	TSet<int32> ChangedItemSlots;
	StackItemsAndFillEmptySlots(RowName, Count, InventoryItems, Result, ChangedItemSlots);

	NotifyInventoryItemsChanged(ChangedItemSlots.Array());

	return Result;
}

void UBaseInventoryComponent::StackItemsAndFillEmptySlots(const FName RowName,
                                                          const int32 Count,
                                                          const TArray<TObjectPtr<UBaseInventoryItem>>& SlotBox,
                                                          FAddItemResult& Result,
                                                          TSet<int32>& ChangedItemSlots)
{
	const FItemInfoData* ItemData = ItemGimmickSubsystem->FindItemInfoData(RowName);
	if (!CR4S_VALIDATE(LogInventory, ItemData),
		!CR4S_VALIDATE(LogInventory, IsItemAllowedByFilter(ItemData->ItemTags)))
	{
		return;
	}

	Result.bSuccess = true;
	int32 RemainingCount = Count;

	TArray<UBaseInventoryItem*> SameItems;
	TArray<UBaseInventoryItem*> EmptySlots;

	GetSameItemSlotsAndEmptySlots(RowName, SlotBox, SameItems, EmptySlots);

	// Try to stack items with existing ones of the same type
	for (UBaseInventoryItem* SameInventoryItem : SameItems)
	{
		if (RemainingCount <= 0)
		{
			break;
		}

		if (SameInventoryItem->GetCurrentStackCount() < ItemData->MaxStackCount)
		{
			const int32 SameInventoryItemCount = SameInventoryItem->GetCurrentStackCount();
			const int32 CanAddCount = ItemData->MaxStackCount - SameInventoryItemCount;
			const int32 ActualAddCount = FMath::Min(CanAddCount, RemainingCount);

			SameInventoryItem->SetCurrentStackCount(SameInventoryItemCount + ActualAddCount);
			RemainingCount -= ActualAddCount;
			Result.AddedCount += ActualAddCount;

			ChangedItemSlots.Add(SameInventoryItem->GetSlotIndex());
		}
	}

	// If there are still items remaining, put them in empty slots
	if (RemainingCount > 0)
	{
		for (UBaseInventoryItem* EmptyInventoryItem : EmptySlots)
		{
			if (RemainingCount <= 0)
			{
				break;
			}

			const int32 ActualAddCount = FMath::Min(RemainingCount, ItemData->MaxStackCount);
			RemainingCount -= ActualAddCount;
			Result.AddedCount += ActualAddCount;

			EmptyInventoryItem->SetInventoryItemData(
				FInventoryItemData(RowName, *ItemData),
				ActualAddCount);

			ChangedItemSlots.Add(EmptyInventoryItem->GetSlotIndex());
		}
	}

	Result.RemainingCount = RemainingCount;
}

bool UBaseInventoryComponent::IsItemAllowedByFilter(const FGameplayTagContainer& ItemTags) const
{
	if (!IsValid(FilterData))
	{
		return true;
	}

	return FilterData->IsAllowedItem(ItemTags);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UBaseInventoryComponent::GetSameItemSlotsAndEmptySlots(const FName& InRowName,
                                                            const TArray<TObjectPtr<UBaseInventoryItem>>& SlotBox,
                                                            TArray<UBaseInventoryItem*>& OutSameItems,
                                                            TArray<UBaseInventoryItem*>& OutEmptySlots)
{
	OutSameItems.Empty();
	OutEmptySlots.Empty();

	for (UBaseInventoryItem* Item : SlotBox)
	{
		if (!CR4S_VALIDATE(LogInventory, IsValid(Item)))
		{
			continue;
		}

		if (!Item->HasItemData())
		{
			OutEmptySlots.Add(Item);
		}
		else if (Item->GetInventoryItemData()->RowName == InRowName)
		{
			OutSameItems.Add(Item);
		}
	}
}

UBaseInventoryItem* UBaseInventoryComponent::GetItemDataByIndex(const int32 Index) const
{
	return CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(Index)) ? InventoryItems[Index] : nullptr;
}

int32 UBaseInventoryComponent::GetItemCountByRowName(const FName RowName) const
{
	int32 Count = 0;

	for (const UBaseInventoryItem* Item : InventoryItems)
	{
		if (IsValid(Item) && Item->GetInventoryItemData()->RowName == RowName)
		{
			Count += Item->GetCurrentStackCount();
		}
	}

	return Count;
}

void UBaseInventoryComponent::RemoveItem(const FName RowName, const int32 Count)
{
	int32 RemainingCount = Count;
	for (UBaseInventoryItem* Item : InventoryItems)
	{
		if (RemainingCount <= 0)
		{
			return;
		}

		if (IsValid(Item) && Item->GetInventoryItemData()->RowName == RowName)
		{
			const int32 ItemCount = Item->GetCurrentStackCount();
			const int32 RemoveCount = FMath::Min(ItemCount, RemainingCount);

			Item->SetCurrentStackCount(ItemCount - RemoveCount);
			RemainingCount -= RemoveCount;

			NotifyInventoryItemChanged(Item->GetSlotIndex());
		}
	}
}

void UBaseInventoryComponent::RemoveItemByIndex(const int32 Index)
{
	if (InventoryItems.IsValidIndex(Index))
	{
		InventoryItems[Index]->SetCurrentStackCount(0);
		NotifyInventoryItemChanged(Index);
	}
}

void UBaseInventoryComponent::SortInventoryItems()
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	TSet<int32> ChangedItemSlots;
	ChangedItemSlots.Reserve(MaxItemSlot);

	TMap<FName, int32> TotalCounts;
	for (UBaseInventoryItem* Item : InventoryItems)
	{
		if (!CR4S_VALIDATE(LogInventory, IsValid(Item)))
		{
			continue;
		}

		if (Item->HasItemData())
		{
			TotalCounts.FindOrAdd(Item->GetInventoryItemData()->RowName) += Item->GetCurrentStackCount();
			Item->SetCurrentStackCount(0);
			ChangedItemSlots.Add(Item->GetSlotIndex());
		}
	}

	int32 SlotIndex = 0;
	for (const FName& RowName : ItemGimmickSubsystem->GetItemDataRowNames())
	{
		const int32* TotalCount = TotalCounts.Find(RowName);
		if (TotalCount == nullptr || *TotalCount <= 0)
		{
			continue;
		}

		int32 RemainingCount = *TotalCount;
		const FItemInfoData* ItemData = ItemGimmickSubsystem->FindItemInfoData(RowName);
		const int32 ItemMaxCount = ItemData->MaxStackCount;

		while (RemainingCount > 0 && SlotIndex < InventoryItems.Num())
		{
			UBaseInventoryItem* Item = InventoryItems[SlotIndex];
			if (!CR4S_VALIDATE(LogInventory, IsValid(Item)))
			{
				SlotIndex++;
				continue;
			}

			const int32 ActualAddCount = FMath::Min(RemainingCount, ItemMaxCount);
			Item->SetInventoryItemData(FInventoryItemData(RowName, *ItemData), ActualAddCount);

			RemainingCount -= ActualAddCount;
			ChangedItemSlots.Add(SlotIndex);
			SlotIndex++;
		}
	}

	NotifyInventoryItemsChanged(ChangedItemSlots.Array());
}

void UBaseInventoryComponent::NotifyInventoryItemChanged(const int32 ItemIndex) const
{
	if (CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(ItemIndex)))
	{
		if (OnItemSlotChanged.IsBound())
		{
			OnItemSlotChanged.Broadcast(InventoryItems[ItemIndex]);
		}
	}
}

void UBaseInventoryComponent::NotifyInventoryItemsChanged(const TArray<int32>& ChangedItemIndexes) const
{
	for (const int32 ItemSlotIndex : ChangedItemIndexes)
	{
		NotifyInventoryItemChanged(ItemSlotIndex);
	}
}
