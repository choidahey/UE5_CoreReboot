#include "InventorySystemComponent.h"

#include "Character/Characters/PlayerCharacter.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/UI/InventoryWidget.h"


UInventorySystemComponent::UInventorySystemComponent()
	: MaxInventorySlot(1)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventorySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryItems.SetNum(MaxInventorySlot);

	for (int32 Index = 0; Index < MaxInventorySlot; ++Index)
	{
		InventoryItems[Index].SlotIndex = Index;
	}

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (IsValid(GameInstance))
	{
		ItemGimmickSubsystem = GameInstance->GetSubsystem<UItemGimmickSubsystem>();
	}

	CreateInventoryWidget();

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (IsValid(OwnerPawn))
	{
		OwnerPlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	}
}

FAddItemResult UInventorySystemComponent::AddItem(const FInventoryItem& InInventoryItem)
{
	FAddItemResult Result;
	Result.RemainingCount = 0;

	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return Result;
	}

	const FBaseItemData* ItemData = ItemGimmickSubsystem->FindItemData(InInventoryItem.RowName);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData is invalid"));
		return Result;
	}

	Result.Success = true;
	int32 RemainingCount = InInventoryItem.Count;

	TArray<FInventoryItem*> SameItems;
	TArray<FInventoryItem*> EmptySlots;

	GetInventoryItemsAndEmptySlots(InInventoryItem.RowName, SameItems, EmptySlots);

	TSet<int32> ChangedItemSlots;

	// Try to stack items with existing ones of the same type
	for (FInventoryItem* SameInventoryItem : SameItems)
	{
		if (RemainingCount <= 0)
		{
			break;
		}

		if (SameInventoryItem->Count < ItemData->MaxStack)
		{
			const int32 CanAddCount = ItemData->MaxStack - SameInventoryItem->Count;
			const int32 ActualAddCount = FMath::Min(CanAddCount, RemainingCount);

			SameInventoryItem->Count += ActualAddCount;
			RemainingCount -= ActualAddCount;
			Result.AddedCount += ActualAddCount;

			ChangedItemSlots.Add(SameInventoryItem->SlotIndex);
		}
	}

	// If there are still items remaining, put them in empty slots
	if (RemainingCount > 0)
	{
		for (FInventoryItem* EmptyInventoryItem : EmptySlots)
		{
			if (RemainingCount <= 0)
			{
				break;
			}

			EmptyInventoryItem->RowName = InInventoryItem.RowName;
			if (ItemData->Icon.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Icon is valid"));
				EmptyInventoryItem->Icon = ItemData->Icon.Get();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Icon is invalid"));
				EmptyInventoryItem->Icon = ItemGimmickSubsystem->LoadIcon(ItemData->Icon);
			}

			EmptyInventoryItem->Count = FMath::Min(RemainingCount, ItemData->MaxStack);
			RemainingCount -= EmptyInventoryItem->Count;
			Result.AddedCount += EmptyInventoryItem->Count;

			ChangedItemSlots.Add(EmptyInventoryItem->SlotIndex);
		}
	}

	Result.RemainingCount = RemainingCount;

	NotifyItemSlotsChanged(ChangedItemSlots.Array());

	// If there are still remaining items, spawn them in the world
	if (RemainingCount > 0 && IsValid(ItemGimmickSubsystem))
	{
		SpawnRemainingItems(InInventoryItem.RowName, RemainingCount);
	}

	return Result;
}

void UInventorySystemComponent::GetInventoryItemsAndEmptySlots(
	const FName& InRowName,
	TArray<FInventoryItem*>& OutSameItems,
	TArray<FInventoryItem*>& OutEmptySlots)
{
	OutSameItems.Empty();
	OutEmptySlots.Empty();

	for (FInventoryItem& Item : InventoryItems)
	{
		if (Item.Count == 0)
		{
			OutEmptySlots.Add(&Item);
		}
		else if (Item.RowName == InRowName)
		{
			OutSameItems.Add(&Item);
		}
	}
}

void UInventorySystemComponent::SpawnRemainingItems(const FName& ItemRowName, const int32 Count) const
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Count is invalid"));
		return;
	}

	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return;
	}

	FVector SpawnLocation = GetOwner()->GetActorLocation();

	SpawnLocation.X += FMath::RandRange(-100.0f, 100.0f);
	SpawnLocation.Y += FMath::RandRange(-100.0f, 100.0f);

	ABaseGimmick* SpawnedGimmick = ItemGimmickSubsystem->SpawnGimmickByRowName(ItemRowName, SpawnLocation);
	if (IsValid(SpawnedGimmick))
	{
	}
}

bool UInventorySystemComponent::SwapItems(const int32 FromIndex, const int32 ToIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Try to swap items from %d to %d"), FromIndex, ToIndex);

	if (!InventoryItems.IsValidIndex(FromIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("FromIndex is invalid"));
		return false;
	}

	if (!InventoryItems.IsValidIndex(ToIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("ToIndex is invalid"));
		return false;
	}

	InventoryItems[FromIndex].SwapItem(InventoryItems[ToIndex]);

	NotifyItemSlotsChanged({FromIndex, ToIndex});

	return true;
}

void UInventorySystemComponent::MergeItems(const int32 FromIndex, const int32 ToIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Try to merge items from %d to %d"), FromIndex, ToIndex);

	if (!InventoryItems.IsValidIndex(FromIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("FromIndex is invalid"));
		return;
	}

	if (!InventoryItems.IsValidIndex(ToIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("ToIndex is invalid"));
		return;
	}

	FInventoryItem& FromItem = InventoryItems[FromIndex];
	FInventoryItem& ToItem = InventoryItems[ToIndex];

	const FBaseItemData* ItemData = FindItemDataFromDataTable(FromItem.RowName);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemData is invalid"));
		return;
	}

	if (ToItem.Count < ItemData->MaxStack)
	{
		const int32 CanAddCount = ItemData->MaxStack - ToItem.Count;
		const int32 ActualAddCount = FMath::Min(CanAddCount, FromItem.Count);

		ToItem.Count += ActualAddCount;
		FromItem.Count -= ActualAddCount;
	}

	NotifyItemSlotsChanged({FromIndex, ToIndex});
}

const FBaseItemData* UInventorySystemComponent::FindItemDataFromDataTable(const FName& RowName) const
{
	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
		return nullptr;
	}

	return ItemGimmickSubsystem->FindItemData(RowName);
}

const FInventoryItem* UInventorySystemComponent::GetItemDataByIndex(const int32 Index) const
{
	if (!InventoryItems.IsValidIndex(Index))
	{
		return nullptr;
	}

	return &InventoryItems[Index];
}

void UInventorySystemComponent::RemoveItemAtIndex(const int32 Index, const int32 RemoveCount)
{
	if (!InventoryItems.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is invalid"));
		return;
	}

	if (RemoveCount <= 0)
	{
		InventoryItems[Index].Count = 0;
	}
	else
	{
		InventoryItems[Index].Count = FMath::Max(0, InventoryItems[Index].Count - RemoveCount);		
	}
	
	NotifyItemSlotChanged(Index);
}

void UInventorySystemComponent::SortInventoryItems()
{
    if (!IsValid(ItemGimmickSubsystem))
    {
       UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is invalid"));
       return;
    }
	
    const int32 OriginalSize = InventoryItems.Num();
	
    TMap<FName, FInventoryItem> CombinedMap;
    CombinedMap.Reserve(InventoryItems.Num());

    for (const FInventoryItem& Item : InventoryItems)
    {
       if (Item.Count == 0)
       {
          continue;
       }
       
       if (FInventoryItem* Existing = CombinedMap.Find(Item.RowName))
       {
          Existing->Count += Item.Count;
       }
       else
       {
          CombinedMap.Add(Item.RowName, Item);
       }
    }
	
    TArray<FInventoryItem> NewItems;
    NewItems.Reserve(OriginalSize);
	
    for (const FName& RowName : ItemGimmickSubsystem->GetItemDataRowNames())
    {
       const FInventoryItem* BaseItem = CombinedMap.Find(RowName);
       if (!BaseItem)
       {
          continue;
       }

       int32 TotalCount = BaseItem->Count;

       const FBaseItemData* ItemData = FindItemDataFromDataTable(RowName);
       if (!ItemData)
       {
          continue;
       }
    	
       while (TotalCount > 0 && NewItems.Num() < OriginalSize)
       {
          FInventoryItem SlotItem = *BaseItem;
          SlotItem.Count = FMath::Min(TotalCount, ItemData->MaxStack);
          NewItems.Add(SlotItem);
          TotalCount -= SlotItem.Count;
       }
    }
	
    while (NewItems.Num() < OriginalSize)
    {
       FInventoryItem EmptyItem;
       EmptyItem.RowName = "";
       EmptyItem.Count = 0;
       NewItems.Add(EmptyItem);
    }
	
    TArray<int32> ChangedItemSlots;
    ChangedItemSlots.Reserve(OriginalSize);

    for (int32 Index = 0; Index < OriginalSize; ++Index)
    {
       const FInventoryItem& OldItem = InventoryItems[Index];
       const FInventoryItem& NewItem = NewItems[Index];

       if (OldItem.RowName != NewItem.RowName || OldItem.Count != NewItem.Count)
       {
          ChangedItemSlots.Add(Index);
       }
    }
	
    InventoryItems = MoveTemp(NewItems);
	
    NotifyItemSlotsChanged(ChangedItemSlots);
}

void UInventorySystemComponent::OpenInventory()
{
	if (IsValid(InventoryWidgetInstance) && IsValid(OwnerPlayerController))
	{
		OwnerPlayerController->SetShowMouseCursor(true);

		if (!InventoryWidgetInstance->IsInViewport())
		{
			InventoryWidgetInstance->AddToViewport();
			OwnerPlayerController->SetInputMode(FInputModeUIOnly());
		}
	}
}

void UInventorySystemComponent::CloseInventory()
{
	if (IsValid(InventoryWidgetInstance) && IsValid(OwnerPlayerController))
	{
		OwnerPlayerController->SetShowMouseCursor(false);

		if (InventoryWidgetInstance->IsInViewport())
		{
			InventoryWidgetInstance->RemoveFromParent();
			OwnerPlayerController->SetInputMode(FInputModeGameOnly());
		}
	}
}

void UInventorySystemComponent::CreateInventoryWidget()
{
	const APlayerCharacter* OwnerPlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!IsValid(OwnerPlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is invalid"));
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(OwnerPlayerCharacter->GetController());
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is invalid"));
		return;
	}

	if (!IsValid(InventoryWidgetClass))
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetClass is invalid"));
	}

	InventoryWidgetInstance = CreateWidget<UInventoryWidget>(PlayerController, InventoryWidgetClass);
	InventoryWidgetInstance->InitInventoryWidget(this, MaxInventorySlot);
}

void UInventorySystemComponent::NotifyItemSlotChanged(const int32 ItemSlotIndex) const
{
	if (InventoryItems.IsValidIndex(ItemSlotIndex))
	{
		if (OnItemSlotChanged.IsBound())
		{
			OnItemSlotChanged.Broadcast(ItemSlotIndex
			                            , InventoryItems[ItemSlotIndex].RowName
			                            , InventoryItems[ItemSlotIndex].Icon
			                            , InventoryItems[ItemSlotIndex].Count);
		}
	}
}

void UInventorySystemComponent::NotifyItemSlotsChanged(const TArray<int32>& ChangedItemSlots) const
{
	for (const int32 ItemSlotIndex : ChangedItemSlots)
	{
		NotifyItemSlotChanged(ItemSlotIndex);
	}
}
