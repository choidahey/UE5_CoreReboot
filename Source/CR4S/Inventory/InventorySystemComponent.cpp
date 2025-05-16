#include "InventorySystemComponent.h"

#include "Character/Characters/PlayerCharacter.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryWidget.h"


UInventorySystemComponent::UInventorySystemComponent()
	: MaxInventorySlot(1)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventorySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryItems.SetNum(MaxInventorySlot);

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (IsValid(GameInstance))
	{
		ItemGimmickSubsystem = GameInstance->GetSubsystem<UItemGimmickSubsystem>();
	}

	CreateInventoryWidget();
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

	TArray<int32> ChangedItemSlots;

	// Try to stack items with existing ones of the same type
	for (int32 Index = 0; Index < SameItems.Num(); ++Index)
	{
		if (RemainingCount <= 0)
		{
			break;
		}

		FInventoryItem* SameInventoryItem = SameItems[Index];

		if (SameInventoryItem->Count < ItemData->MaxStack)
		{
			const int32 CanAddCount = ItemData->MaxStack - SameInventoryItem->Count;
			const int32 ActualAddCount = FMath::Min(CanAddCount, RemainingCount);

			SameInventoryItem->Count += ActualAddCount;
			RemainingCount -= ActualAddCount;
			Result.AddedCount += ActualAddCount;

			ChangedItemSlots.AddUnique(Index);
		}
	}

	// If there are still items remaining, put them in empty slots
	if (RemainingCount > 0)
	{
		for (int32 Index = 0; Index < EmptySlots.Num(); ++Index)
		{
			if (RemainingCount <= 0)
			{
				break;
			}

			FInventoryItem* EmptyInventoryItem = EmptySlots[Index];

			EmptyInventoryItem->RowName = InInventoryItem.RowName;
			EmptyInventoryItem->Count = FMath::Min(RemainingCount, ItemData->MaxStack);
			RemainingCount -= EmptyInventoryItem->Count;
			Result.AddedCount += EmptyInventoryItem->Count;

			ChangedItemSlots.AddUnique(Index);
		}
	}

	Result.RemainingCount = RemainingCount;

	NotifyItemSlotsChanged(ChangedItemSlots);

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

	for (auto& Item : InventoryItems)
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

bool UInventorySystemComponent::SwapItemIndex(const int32 FromIndex, const int32 ToIndex)
{
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

	const FInventoryItem TempItem = InventoryItems[FromIndex];
	InventoryItems[FromIndex] = InventoryItems[ToIndex];
	InventoryItems[ToIndex] = TempItem;

	NotifyItemSlotsChanged({FromIndex, ToIndex});

	return true;
}

void UInventorySystemComponent::RemoveItemAt(const int32 Index)
{
	if (!InventoryItems.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is invalid"));
		return;
	}

	InventoryItems[Index].Count = 0;
	NotifyItemSlotChanged(Index);
}

void UInventorySystemComponent::OpenInventory()
{
	if (IsValid(InventoryWidgetInstance))
	{
		InventoryWidgetInstance->AddToViewport();
	}
}

void UInventorySystemComponent::CloseInventory()
{
	if (IsValid(InventoryWidgetInstance))
	{
		InventoryWidgetInstance->RemoveFromParent();
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
}

void UInventorySystemComponent::NotifyItemSlotChanged(const int32 ItemSlotIndex) const
{
	if (InventoryItems.IsValidIndex(ItemSlotIndex))
	{
		if (OnItemSlotChanged.IsBound())
		{
			OnItemSlotChanged.Broadcast(ItemSlotIndex);
		}
	}
}

void UInventorySystemComponent::NotifyItemSlotsChanged(const TArray<int32>& ChangedItemSlots) const
{
	if (ChangedItemSlots.Num() > InventoryItems.Num() / 2)
	{
		if (OnInventoryChanged.IsBound())
		{
			OnInventoryChanged.Broadcast();
		}
	}
	else
	{
		for (const int32 ItemSlotIndex : ChangedItemSlots)
		{
			NotifyItemSlotChanged(ItemSlotIndex);
		}
	}
}
