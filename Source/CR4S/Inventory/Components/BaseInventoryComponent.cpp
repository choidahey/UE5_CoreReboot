#include "BaseInventoryComponent.h"

#include "CR4S.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

UBaseInventoryComponent::UBaseInventoryComponent()
	: MaxInventorySlot(10),
	  bIsInitializedInventorySize(false)
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

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (CR4S_VALIDATE(LogInventory, IsValid(GameInstance)))
	{
		ItemGimmickSubsystem = GameInstance->GetSubsystem<UItemGimmickSubsystem>();
	}

	InitInventorySize();
}

void UBaseInventoryComponent::InitInventorySize()
{
	if (CR4S_VALIDATE(LogInventory, bIsInitializedInventorySize))
	{
		return;
	}

	InventoryItems.Reserve(MaxInventorySlot);
	for (int32 Index = 0; Index < MaxInventorySlot; ++Index)
	{
		UBaseInventoryItem* Item = NewObject<UBaseInventoryItem>(this);
		Item->InitInventoryItem(OwnerActor, Index);
		InventoryItems.Add(Item);
	}

	bIsInitializedInventorySize = true;
}

FAddItemResult UBaseInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	FAddItemResult Result;
	Result.RemainingCount = 0;

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
	if (!CR4S_VALIDATE(LogInventory, ItemData))
	{
		return;
	}

	Result.Success = true;
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

void UBaseInventoryComponent::SortInventoryItems()
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	TSet<int32> ChangedItemSlots;
	ChangedItemSlots.Reserve(MaxInventorySlot);

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

void UBaseInventoryComponent::SwapItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(FromItem)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(ToItem)))
	{
		return;
	}

	FromItem->SwapData(ToItem);

	NotifyInventoryItemsChanged({FromItem->GetSlotIndex(), ToItem->GetSlotIndex()});
}

void UBaseInventoryComponent::MergeItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(FromItem)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(ToItem)))
	{
		return;
	}

	const int32 ToItemCount = ToItem->GetCurrentStackCount();
	const int32 MaxStackCount = ToItem->GetInventoryItemData()->ItemInfoData.MaxStackCount;
	if (ToItemCount < MaxStackCount)
	{
		const int32 CanAddCount = MaxStackCount - ToItemCount;
		const int32 ActualAddCount = FMath::Min(CanAddCount, FromItem->GetCurrentStackCount());

		ToItem->SetCurrentStackCount(ToItemCount + ActualAddCount);
		FromItem->SetCurrentStackCount(FromItem->GetCurrentStackCount() - ActualAddCount);
	}

	NotifyInventoryItemsChanged({FromItem->GetSlotIndex(), ToItem->GetSlotIndex()});
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
