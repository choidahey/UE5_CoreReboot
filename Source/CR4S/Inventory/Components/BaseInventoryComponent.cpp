#include "BaseInventoryComponent.h"

#include "CR4S.h"
#include "GameplayTagsManager.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryFilterData/InventoryFilterData.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/InventoryItem/ConsumableInventoryItem.h"
#include "Inventory/InventoryItem/ToolInventoryItem.h"

UBaseInventoryComponent::UBaseInventoryComponent()
	: MaxInventorySize(10),
	  InventoryTitleText(FText::FromString("INVENTORY"))
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InitInventory();
}

void UBaseInventoryComponent::InitInventory()
{
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

	InventoryItems.Init(nullptr, MaxInventorySize);
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

		if (Result.bSuccess && Result.RemainingCount > 0)
		{
			RemainingItems.FindOrAdd(RowName) += Result.RemainingCount;
		}
	}

	if (RemainingItems.Num() > 0)
	{
		ItemGimmickSubsystem->SpawnItemPouch(OwnerActor, RemainingItems);
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
	StackItemsAndFillEmptySlots(RowName, Count, Result, ChangedItemSlots);

	NotifyInventoryItemsChanged(ChangedItemSlots.Array());

	return Result;
}

void UBaseInventoryComponent::StackItemsAndFillEmptySlots(const FName RowName,
                                                          const int32 Count,
                                                          FAddItemResult& Result,
                                                          TSet<int32>& ChangedItemSlots)
{
	const FItemInfoData* ItemData = ItemGimmickSubsystem->FindItemInfoData(RowName);
	if (!CR4S_VALIDATE(LogInventory, ItemData) ||
		!CR4S_VALIDATE(LogInventory, IsItemAllowedByFilter(ItemData->ItemTags)))
	{
		return;
	}

	Result.bSuccess = true;
	int32 RemainingCount = Count;

	TArray<int32> SameItemsIndex;
	TArray<int32> EmptySlotsIndex;

	GetSameItemSlotsAndEmptySlots(RowName, InventoryItems, SameItemsIndex, EmptySlotsIndex);

	// Try to stack items with existing ones of the same type
	for (const int32 Index : SameItemsIndex)
	{
		if (RemainingCount <= 0)
		{
			break;
		}

		if (!InventoryItems.IsValidIndex(Index))
		{
			continue;
		}

		UBaseInventoryItem* SameInventoryItem = InventoryItems[Index];
		if (!CR4S_VALIDATE(LogInventory, IsValid(SameInventoryItem)))
		{
			continue;
		}

		if (SameInventoryItem->GetCurrentStackCount() < ItemData->MaxStackCount)
		{
			const int32 SameInventoryItemCount = SameInventoryItem->GetCurrentStackCount();
			const int32 CanAddCount = ItemData->MaxStackCount - SameInventoryItemCount;
			const int32 ActualAddCount = FMath::Min(CanAddCount, RemainingCount);

			SameInventoryItem->SetCurrentStackCount(SameInventoryItemCount + ActualAddCount);
			RemainingCount -= ActualAddCount;
			Result.AddedCount += ActualAddCount;

			ChangedItemSlots.Add(Index);
		}
	}

	// If there are still items remaining, put them in empty slots
	if (RemainingCount > 0)
	{
		for (const int32 Index : EmptySlotsIndex)
		{
			if (RemainingCount <= 0)
			{
				break;
			}

			if (!InventoryItems.IsValidIndex(Index))
			{
				continue;
			}

			UBaseInventoryItem* EmptyInventoryItem = CreateInventoryItem(ItemData->ItemTags);
			if (!CR4S_VALIDATE(LogInventory, EmptyInventoryItem))
			{
				continue;
			}

			InventoryItems[Index] = EmptyInventoryItem;

			const int32 ActualAddCount = FMath::Min(RemainingCount, ItemData->MaxStackCount);
			RemainingCount -= ActualAddCount;
			Result.AddedCount += ActualAddCount;

			EmptyInventoryItem->InitInventoryItem(
				this,
				FInventoryItemData(Index, RowName, *ItemData),
				ActualAddCount);

			ChangedItemSlots.Add(Index);
		}
	}

	Result.RemainingCount = RemainingCount;
}

UBaseInventoryItem* UBaseInventoryComponent::CreateInventoryItem(const FGameplayTagContainer& ItemTags)
{
	FGameplayTag ItemTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Item.Tools"));
	if (ItemTags.HasTag(ItemTag))
	{
		return NewObject<UToolInventoryItem>(this);
	}

	ItemTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Item.Crops"));
	if (ItemTags.HasTag(ItemTag))
	{
		return NewObject<UConsumableInventoryItem>(this);
	}

	return NewObject<UBaseInventoryItem>(this);
}

bool UBaseInventoryComponent::IsItemAllowedByFilter(const FGameplayTagContainer& ItemTags) const
{
	if (!IsValid(FilterData))
	{
		return true;
	}

	return FilterData->IsAllowedItem(ItemTags);
}

int32 UBaseInventoryComponent::GetUseSlotCount()
{
	int32 Result = 0;
	for (const UBaseInventoryItem* InventoryItem : InventoryItems)
	{
		if (IsValid(InventoryItem))
		{
			Result++;
		}
	}

	return Result;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UBaseInventoryComponent::GetSameItemSlotsAndEmptySlots(const FName& InRowName,
                                                            const TArray<TObjectPtr<UBaseInventoryItem>>& SlotBox,
                                                            TArray<int32>& OutSameItemsIndex,
                                                            TArray<int32>& OutEmptySlotsIndex)
{
	OutSameItemsIndex.Empty();
	OutEmptySlotsIndex.Empty();

	for (int32 Index = 0; Index < SlotBox.Num(); Index++)
	{
		const UBaseInventoryItem* Item = SlotBox[Index];
		if (!IsValid(Item))
		{
			OutEmptySlotsIndex.Add(Index);
		}
		else if (Item->GetInventoryItemData()->RowName == InRowName)
		{
			OutSameItemsIndex.Add(Index);
		}
	}
}

UBaseInventoryItem* UBaseInventoryComponent::GetInventoryItemByIndex(const int32 Index) const
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

void UBaseInventoryComponent::SwapItem(UBaseInventoryComponent* FromInventoryComponent,
                                       const int32 FromItemIndex,
                                       const int32 ToItemIndex)
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(FromInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventory, FromInventoryComponent->InventoryItems.IsValidIndex(FromItemIndex)) ||
		!CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(ToItemIndex)))
	{
		return;
	}

	UBaseInventoryItem* FromItem = FromInventoryComponent->InventoryItems[FromItemIndex];
	UBaseInventoryItem* ToItem = InventoryItems[ToItemIndex];

	if (IsValid(FromItem))
	{
		FromItem->UpdateInventoryItem(this);
		FromInventoryComponent->InventoryItems[FromItemIndex]->ChangeSlotIndex(ToItemIndex);
	}

	if (IsValid(ToItem))
	{
		ToItem->UpdateInventoryItem(FromInventoryComponent);
		InventoryItems[ToItemIndex]->ChangeSlotIndex(FromItemIndex);
	}

	Swap(FromInventoryComponent->InventoryItems[FromItemIndex], InventoryItems[ToItemIndex]);

	NotifyInventoryItemChanged(ToItemIndex);
	FromInventoryComponent->NotifyInventoryItemChanged(FromItemIndex);
}

void UBaseInventoryComponent::MergeItem(UBaseInventoryComponent* FromInventoryComponent,
                                        const int32 FromItemIndex,
                                        const int32 ToItemIndex)
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(FromInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventory, FromInventoryComponent->InventoryItems.IsValidIndex(FromItemIndex)) ||
		!CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(ToItemIndex)))
	{
		return;
	}

	UBaseInventoryItem* ToItem = InventoryItems[ToItemIndex];
	UBaseInventoryItem* FromItem = FromInventoryComponent->InventoryItems[FromItemIndex];

	const int32 ToItemCount = ToItem->GetCurrentStackCount();
	const int32 MaxStackCount = ToItem->GetMaxStackCount();
	if (ToItemCount < MaxStackCount)
	{
		const int32 CanAddCount = MaxStackCount - ToItemCount;
		const int32 ActualAddCount = FMath::Min(CanAddCount, FromItem->GetCurrentStackCount());

		ToItem->SetCurrentStackCount(ToItemCount + ActualAddCount);
		FromItem->SetCurrentStackCount(FromItem->GetCurrentStackCount() - ActualAddCount);

		if (FromItem->IsEmpty())
		{
			FromItem = nullptr;
		}

		AveragingFreshness(FromItem, ToItem);
	}

	NotifyInventoryItemChanged(ToItemIndex);
	FromInventoryComponent->NotifyInventoryItemChanged(FromItemIndex);
}

void UBaseInventoryComponent::AveragingFreshness(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem)
{
	UConsumableInventoryItem* FromConsumableInventoryItem = Cast<UConsumableInventoryItem>(FromItem);
	const UConsumableInventoryItem* ToConsumableInventoryItem = Cast<UConsumableInventoryItem>(ToItem);
	if (!IsValid(FromConsumableInventoryItem) ||
		!IsValid(ToConsumableInventoryItem))
	{
		return;
	}

	FromConsumableInventoryItem->AveragingFreshness(ToConsumableInventoryItem->GetRemainingFreshnessTime());
}

void UBaseInventoryComponent::SetInventoryItems(const TArray<UBaseInventoryItem*>& NewInventoryItems)
{
	if (InventoryItems.Num() != NewInventoryItems.Num())
	{
		return;
	}

	InventoryItems = NewInventoryItems;

	for (int32 Index = 0; Index < InventoryItems.Num(); Index++)
	{
		NotifyInventoryItemChanged(Index);
	}
}

void UBaseInventoryComponent::RemoveItemByRowName(const FName RowName, const int32 Count)
{
	if (Count <= 0)
	{
		return;
	}

	int32 RemainingCount = Count;
	for (int32 Index = 0; Index < InventoryItems.Num(); Index++)
	{
		if (RemainingCount <= 0)
		{
			return;
		}

		UBaseInventoryItem* Item = InventoryItems[Index];

		if (IsValid(Item) && Item->GetInventoryItemData()->RowName == RowName)
		{
			const int32 ItemCount = Item->GetCurrentStackCount();

			if (RemainingCount >= ItemCount)
			{
				InventoryItems[Index]->EndPassiveEffect();
				InventoryItems[Index] = nullptr;
				RemainingCount -= ItemCount;
			}
			else
			{
				Item->SetCurrentStackCount(ItemCount - RemainingCount);
				RemainingCount = 0;
			}

			NotifyInventoryItemChanged(Index);
		}
	}
}

void UBaseInventoryComponent::RemoveAllItemByRowName(const FName RowName)
{
	for (int32 Index = 0; Index < InventoryItems.Num(); Index++)
	{
		const UBaseInventoryItem* Item = InventoryItems[Index];
		if (IsValid(Item) && Item->GetInventoryItemData()->RowName == RowName)
		{
			InventoryItems[Index]->EndPassiveEffect();
			InventoryItems[Index] = nullptr;

			NotifyInventoryItemChanged(Index);
		}
	}
}

void UBaseInventoryComponent::RemoveItemByIndex(const int32 Index, const int32 Count)
{
	if (Count == 0)
	{
		return;
	}

	if (InventoryItems.IsValidIndex(Index))
	{
		UBaseInventoryItem* Item = InventoryItems[Index];
		if (IsValid(Item))
		{
			const int32 NewCount = Count <= -1 ? 0 : Item->GetCurrentStackCount() - Count;

			Item->SetCurrentStackCount(NewCount);

			if (Item->IsEmpty())
			{
				InventoryItems[Index]->EndPassiveEffect();
				InventoryItems[Index] = nullptr;
			}

			NotifyInventoryItemChanged(Index);
		}
	}
}

void UBaseInventoryComponent::SortInventoryItems()
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	TSet<int32> ChangedItemSlots;
	ChangedItemSlots.Reserve(MaxInventorySize);

	TMap<FName, int32> TotalCounts;
	for (int32 Index = 0; Index < InventoryItems.Num(); Index++)
	{
		UBaseInventoryItem* Item = InventoryItems[Index];
		if (!IsValid(Item))
		{
			continue;
		}

		TotalCounts.FindOrAdd(Item->GetInventoryItemData()->RowName) += Item->GetCurrentStackCount();

		InventoryItems[Index]->EndPassiveEffect();
		InventoryItems[Index] = nullptr;

		ChangedItemSlots.Add(Index);
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
		if (!CR4S_VALIDATE(LogInventory, ItemData))
		{
			continue;
		}

		const int32 ItemMaxCount = ItemData->MaxStackCount;

		while (RemainingCount > 0 && SlotIndex < InventoryItems.Num())
		{
			UBaseInventoryItem* Item = InventoryItems[SlotIndex];
			if (!IsValid(Item))
			{
				Item = CreateInventoryItem(ItemData->ItemTags);
				InventoryItems[SlotIndex] = Item;
			}

			const int32 ActualAddCount = FMath::Min(RemainingCount, ItemMaxCount);
			Item->InitInventoryItem(this, FInventoryItemData(SlotIndex, RowName, *ItemData), ActualAddCount);

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
		if (OnItemSlotChange.IsBound())
		{
			OnItemSlotChange.Broadcast(ItemIndex, InventoryItems[ItemIndex]);
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
