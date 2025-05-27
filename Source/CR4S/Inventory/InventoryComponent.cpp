#include "InventoryComponent.h"

#include "CR4S.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/UI/InventoryWidget.h"
#include "InventoryItem/BaseInventoryItem.h"
#include "UI/QuickSlotBarWidget.h"
#include "UI/InGame/SurvivalHUD.h"


UInventoryComponent::UInventoryComponent()
	: MaxInventorySlot(50)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<AActor>(GetOwner());
	if (!CR4S_VALIDATE(LogInventory, IsValid(OwnerActor)))
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

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!CR4S_VALIDATE(LogInventory, IsValid(PlayerController)))
	{
		return;
	}

	SurvivalHUD = Cast<ASurvivalHUD>(PlayerController->GetHUD());

	CR4S_VALIDATE(LogInventory, CreateInventoryWidget());
	CR4S_VALIDATE(LogInventory, CreateQuickSlotBarWidget());

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (CR4S_VALIDATE(LogInventory, IsValid(GameInstance)))
	{
		ItemGimmickSubsystem = GameInstance->GetSubsystem<UItemGimmickSubsystem>();
	}
}

FAddItemResult UInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	FAddItemResult Result;
	Result.RemainingCount = 0;

	if (CR4S_VALIDATE(LogInventory, Count <= 0) ||
		!CR4S_VALIDATE(LogInventory, IsValid(ItemGimmickSubsystem)))
	{
		return Result;
	}

	const FItemInfoData* ItemData = ItemGimmickSubsystem->FindItemInfoData(RowName);
	if (!CR4S_VALIDATE(LogInventory, ItemData))
	{
		return Result;
	}

	Result.Success = true;
	int32 RemainingCount = Count;

	TArray<UBaseInventoryItem*> SameItems;
	TArray<UBaseInventoryItem*> EmptySlots;

	GetInventoryItemsAndEmptySlots(RowName, SameItems, EmptySlots);

	TSet<int32> ChangedItemSlots;

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

	NotifyItemSlotsChanged(ChangedItemSlots.Array());

	return Result;
}

void UInventoryComponent::GetInventoryItemsAndEmptySlots(const FName& InRowName,
                                                         TArray<UBaseInventoryItem*>& OutSameItems,
                                                         TArray<UBaseInventoryItem*>& OutEmptySlots)
{
	OutSameItems.Empty();
	OutEmptySlots.Empty();

	for (UBaseInventoryItem* Item : InventoryItems)
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

bool UInventoryComponent::SwapItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(FromItem)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(ToItem)))
	{
		return false;
	}

	FromItem->SwapData(ToItem);

	NotifyItemSlotsChanged({FromItem->GetSlotIndex(), ToItem->GetSlotIndex()});

	return true;
}

void UInventoryComponent::MergeItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const
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

	NotifyItemSlotsChanged({FromItem->GetSlotIndex(), ToItem->GetSlotIndex()});
}

UBaseInventoryItem* UInventoryComponent::GetItemDataByIndex(const int32 Index) const
{
	if (!CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(Index)))
	{
		return nullptr;
	}

	return InventoryItems[Index];
}

void UInventoryComponent::RemoveItemAtIndex(const int32 Index, const int32 RemoveCount)
{
	if (!CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(Index)))
	{
		return;
	}

	UBaseInventoryItem* TargetItem = InventoryItems[Index];

	if (RemoveCount <= 0)
	{
		TargetItem->SetInventoryItemData(FInventoryItemData(), 0);
	}
	else
	{
		const int32 NewCount = FMath::Max(0, TargetItem->GetCurrentStackCount() - RemoveCount);
		TargetItem->SetCurrentStackCount(NewCount);
	}

	NotifyItemSlotChanged(Index);
}

void UInventoryComponent::SortInventoryItems()
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

	NotifyItemSlotsChanged(ChangedItemSlots.Array());
}

void UInventoryComponent::OpenInventory() const
{
	if (!IsValidWidgetRef() ||
		!CR4S_VALIDATE(LogInventory, InventoryWidgetInstance->GetVisibility() == ESlateVisibility::Collapsed) ||
		!CR4S_VALIDATE(LogInventory, QuickSlotBarWidgetInstance->GetVisibility() == ESlateVisibility::Visible))
	{
		return;
	}

	SurvivalHUD->ToggleWidget(InventoryWidgetInstance);
	SurvivalHUD->ToggleWidget(QuickSlotBarWidgetInstance);
	SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly);
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UInventoryComponent::CloseInventory()
{
	if (!IsValidWidgetRef() ||
		!CR4S_VALIDATE(LogInventory, InventoryWidgetInstance->GetVisibility() == ESlateVisibility::Visible) ||
		!CR4S_VALIDATE(LogInventory, QuickSlotBarWidgetInstance->GetVisibility() == ESlateVisibility::Collapsed))
	{
		return;
	}

	SurvivalHUD->ToggleWidget(InventoryWidgetInstance);
	SurvivalHUD->ToggleWidget(QuickSlotBarWidgetInstance);
	SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
}

bool UInventoryComponent::CreateInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryWidgetClass)))
	{
		return false;
	}

	InventoryWidgetInstance = SurvivalHUD->CreateAndAddWidget(InventoryWidgetClass, 0, ESlateVisibility::Collapsed);

	if (!CR4S_VALIDATE(LogInventory, IsValid(InventoryWidgetInstance)))
	{
		return false;
	}

	InventoryWidgetInstance->InitInventoryWidget(this);

	return true;
}

bool UInventoryComponent::IsValidWidgetRef() const
{
	return CR4S_VALIDATE(LogInventory, IsValid(SurvivalHUD)) &&
		CR4S_VALIDATE(LogInventory, IsValid(InventoryWidgetInstance)) &&
		CR4S_VALIDATE(LogInventory, IsValid(QuickSlotBarWidgetInstance));
}

bool UInventoryComponent::CreateQuickSlotBarWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotBarWidgetClass)))
	{
		return false;
	}

	QuickSlotBarWidgetInstance = SurvivalHUD->CreateAndAddWidget(QuickSlotBarWidgetClass, 0, ESlateVisibility::Visible);

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotBarWidgetInstance)))
	{
		return false;
	}

	QuickSlotBarWidgetInstance->InitWidget(this);

	return true;
}

void UInventoryComponent::NotifyItemSlotChanged(const int32 ItemSlotIndex) const
{
	if (CR4S_VALIDATE(LogInventory, InventoryItems.IsValidIndex(ItemSlotIndex)))
	{
		if (OnItemSlotChanged.IsBound())
		{
			OnItemSlotChanged.Broadcast(InventoryItems[ItemSlotIndex]);
		}
	}
}

void UInventoryComponent::NotifyItemSlotsChanged(const TArray<int32>& ChangedItemSlots) const
{
	for (const int32 ItemSlotIndex : ChangedItemSlots)
	{
		NotifyItemSlotChanged(ItemSlotIndex);
	}
}
