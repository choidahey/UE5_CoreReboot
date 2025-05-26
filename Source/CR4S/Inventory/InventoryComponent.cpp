#include "InventoryComponent.h"

#include "CR4S.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/UI/InventoryWidget.h"
#include "InventoryItem/BaseInventoryItem.h"
#include "UI/InGame/SurvivalHUD.h"


UInventoryComponent::UInventoryComponent()
	: MaxInventorySlot(30)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	CR4S_Log(LogTemp, Warning, TEXT("Begin"));

	OwnerActor = Cast<AActor>(GetOwner());
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerActor is invalid"));
		return;
	}

	InventoryItems.Reserve(MaxInventorySlot);
	for (int32 Index = 0; Index < MaxInventorySlot; ++Index)
	{
		UBaseInventoryItem* Item = NewObject<UBaseInventoryItem>(this);
		Item->InitInventoryItem(OwnerActor, Index);
		InventoryItems.Add(Item);
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is invalid"));
		return;
	}

	SurvivalHUD = Cast<ASurvivalHUD>(PlayerController->GetHUD());

	if (!CreateInventoryWidget())
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateInventoryWidget is failed"));
		return;
	}

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (IsValid(GameInstance))
	{
		ItemGimmickSubsystem = GameInstance->GetSubsystem<UItemGimmickSubsystem>();
	}

	CR4S_Log(LogTemp, Warning, TEXT("End"));
}

FAddItemResult UInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	FAddItemResult Result;
	Result.RemainingCount = 0;

	if (Count <= 0)
	{
		CR4S_Log(LogTemp, Warning, TEXT("Count is invalid"));
		return Result;
	}

	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return Result;
	}
	
	const FItemInfoData* ItemData = ItemGimmickSubsystem->FindItemInfoData(RowName);
	const FInventoryItemData NewItem = FInventoryItemData(ItemData->Type, RowName, ItemData->MaxStackCount);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData is invalid"));
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
				FInventoryItemData(ItemData->Type, RowName, ItemData->MaxStackCount),
				ItemData->Info.Icon,
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
		if (!IsValid(Item))
		{
			CR4S_Log(LogTemp, Warning, TEXT("Item is invalid"));
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

void UInventoryComponent::SpawnRemainingItems(const FName& ItemRowName, const int32 Count) const
{
	UE_LOG(LogTemp, Warning, TEXT("Try to spawn %d items of %s"), Count, *ItemRowName.ToString());

	// if (Count <= 0)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Count is invalid"));
	// 	return;
	// }
	//
	// if (!IsValid(ItemGimmickSubsystem))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
	// 	return;
	// }
	//
	// FVector SpawnLocation = GetOwner()->GetActorLocation();
	//
	// SpawnLocation.X += FMath::RandRange(-100.0f, 100.0f);
	// SpawnLocation.Y += FMath::RandRange(-100.0f, 100.0f);
	//
	// ABaseGimmick* SpawnedGimmick = ItemGimmickSubsystem->SpawnGimmickByRowName(ItemRowName, SpawnLocation);
	// if (IsValid(SpawnedGimmick))
	// {
	// }
}

bool UInventoryComponent::SwapItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const
{
	if (!IsValid(FromItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("FromItem is invalid"));	
		return false;
	}

	if (!IsValid(ToItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("ToItem is invalid"));	
		return false;
	}
		
	FromItem->SwapData(ToItem);

	NotifyItemSlotsChanged({FromItem->GetSlotIndex(), ToItem->GetSlotIndex()});

	return true;
}

void UInventoryComponent::MergeItems(UBaseInventoryItem* FromItem, UBaseInventoryItem* ToItem) const
{
	if (!IsValid(FromItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("FromItem is invalid"));	
		return;
	}

	if (!IsValid(ToItem))
	{
		CR4S_Log(LogTemp, Warning, TEXT("ToItem is invalid"));	
		return;
	}
	
	const FItemInfoData* ItemData = FindItemDataFromDataTable(FromItem->GetInventoryItemData()->RowName);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData is invalid"));
		return;
	}

	const int32 ToItemCount = ToItem->GetCurrentStackCount();
	if (ToItemCount < ItemData->MaxStackCount)
	{
		const int32 CanAddCount = ItemData->MaxStackCount - ToItemCount;
		const int32 ActualAddCount = FMath::Min(CanAddCount, FromItem->GetCurrentStackCount());

		ToItem->SetCurrentStackCount(ToItemCount + ActualAddCount);
		FromItem->SetCurrentStackCount(FromItem->GetCurrentStackCount() - ActualAddCount);
	}

	NotifyItemSlotsChanged({FromItem->GetSlotIndex(), ToItem->GetSlotIndex()});
}

const FItemInfoData* UInventoryComponent::FindItemDataFromDataTable(const FName& RowName) const
{
	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return nullptr;
	}

	return ItemGimmickSubsystem->FindItemInfoData(RowName);
}

UBaseInventoryItem* UInventoryComponent::GetItemDataByIndex(const int32 Index) const
{
	if (!InventoryItems.IsValidIndex(Index))
	{
		return nullptr;
	}

	return InventoryItems[Index];
}

void UInventoryComponent::RemoveItemAtIndex(const int32 Index, const int32 RemoveCount)
{
	if (!InventoryItems.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is invalid"));
		return;
	}

	UBaseInventoryItem* TargetItem = InventoryItems[Index];

	if (RemoveCount <= 0)
	{
		TargetItem->SetInventoryItemData(FInventoryItemData(), nullptr, 0);
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
	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return;
	}

	TSet<int32> ChangedItemSlots;
	ChangedItemSlots.Reserve(MaxInventorySlot);

	TMap<FName, int32> TotalCounts;
	for (UBaseInventoryItem* Item : InventoryItems)
	{
		if (!IsValid(Item))
		{
			CR4S_Log(LogTemp, Warning, TEXT("Item is invalid"));
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
		int32* TotalCount = TotalCounts.Find(RowName);
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
			if (!IsValid(Item))
			{
				SlotIndex++;
				continue;
			}

			const int32 ActualAddCount = FMath::Min(RemainingCount, ItemMaxCount);
			Item->SetInventoryItemData(FInventoryItemData(ItemData->Type, RowName, ItemMaxCount), ItemData->Info.Icon,
			                           ActualAddCount);

			RemainingCount -= ActualAddCount;
			ChangedItemSlots.Add(SlotIndex);
			SlotIndex++;
		}
	}

	NotifyItemSlotsChanged(ChangedItemSlots.Array());
}

void UInventoryComponent::OpenInventory() const
{
	if (IsValid(InventoryWidgetInstance) && IsValid(SurvivalHUD))
	{
		SurvivalHUD->ToggleWidget(InventoryWidgetInstance);
		SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly);
	}
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void UInventoryComponent::CloseInventory()
{
	if (IsValid(InventoryWidgetInstance) && IsValid(SurvivalHUD))
	{
		SurvivalHUD->ToggleWidget(InventoryWidgetInstance);
		SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}
}

bool UInventoryComponent::CreateInventoryWidget()
{
	if (!IsValid(SurvivalHUD))
	{
		UE_LOG(LogTemp, Warning, TEXT("SurvivalHUD is invalid"));
		return false;
	}

	if (!IsValid(InventoryWidgetClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetClass is invalid"));
		return false;
	}

	InventoryWidgetInstance = SurvivalHUD->CreateAndAddWidget(InventoryWidgetClass, 0, ESlateVisibility::Collapsed);

	if (!IsValid(InventoryWidgetInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetInstance is invalid"));
		return false;
	}

	InventoryWidgetInstance->InitInventoryWidget(this);

	return true;
}

void UInventoryComponent::NotifyItemSlotChanged(const int32 ItemSlotIndex) const
{
	if (InventoryItems.IsValidIndex(ItemSlotIndex))
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
