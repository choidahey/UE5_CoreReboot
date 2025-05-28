#include "PlayerInventoryComponent.h"

#include "CR4S.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "UI/InGame/SurvivalHUD.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
	: InventoryContainerWidgetOrder(0),
	  QuickSlotCount(10)
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxItemSlot = 50;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	QuickSlotItems.Reserve(QuickSlotCount);
	for (int32 Index = 0; Index < QuickSlotCount; ++Index)
	{
		UBaseInventoryItem* Item = NewObject<UBaseInventoryItem>(this);
		Item->InitInventoryItem(OwnerActor, Index);
		QuickSlotItems.Add(Item);
	}

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!CR4S_VALIDATE(LogInventory, IsValid(PlayerController)))
	{
		return;
	}

	SurvivalHUD = Cast<ASurvivalHUD>(PlayerController->GetHUD());
	if (!CR4S_VALIDATE(LogInventory, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetClass)))
	{
		return;
	}

	InventoryContainerWidgetInstance = SurvivalHUD->CreateAndAddWidget(InventoryContainerWidgetClass,
	                                                                   InventoryContainerWidgetOrder,
	                                                                   ESlateVisibility::Visible);

	if (CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		InventoryContainerWidgetInstance->InitWidget(SurvivalHUD, this);
	}
}

FAddItemResult UPlayerInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	FAddItemResult Result = Super::AddItem(RowName, Count);

	CR4S_Log(LogGimmick, Warning, TEXT("Remaining Test: %d"), Result.RemainingCount);
	
	if (!CR4S_VALIDATE(LogInventory, Result.Success) ||
		!CR4S_VALIDATE(LogInventory, Result.RemainingCount > 0))
	{
		return Result;
	}

	TSet<int32> ChangedItemSlots;
	StackItemsAndFillEmptySlots(RowName, Result.RemainingCount, QuickSlotItems, Result, ChangedItemSlots);

	NotifyQuickSlotItemsChanged(ChangedItemSlots.Array());

	return Result;
}

void UPlayerInventoryComponent::OpenPlayerInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		return;
	}

	InventoryContainerWidgetInstance->OpenPlayerInventoryWidget();
}

void UPlayerInventoryComponent::OpenOtherInventoryWidget(const EInventoryType InventoryType, UBaseInventoryComponent* InventoryComponent)
{
	OpenPlayerInventoryWidget();

	InventoryContainerWidgetInstance->OpenOtherInventoryWidget(InventoryType, InventoryComponent);
}

UBaseInventoryItem* UPlayerInventoryComponent::GetQuickSlotItemDataByIndex(const int32 Index) const
{
	return CR4S_VALIDATE(LogInventory, QuickSlotItems.IsValidIndex(Index)) ? QuickSlotItems[Index] : nullptr;
}

void UPlayerInventoryComponent::NotifyQuickSlotItemChanged(const int32 ItemSlotIndex) const
{
	if (CR4S_VALIDATE(LogInventory, QuickSlotItems.IsValidIndex(ItemSlotIndex)))
	{
		if (OnQuickSlotItemChanged.IsBound())
		{
			OnQuickSlotItemChanged.Broadcast(QuickSlotItems[ItemSlotIndex]);
		}
	}
}

void UPlayerInventoryComponent::NotifyQuickSlotItemsChanged(const TArray<int32>& ChangedItemSlots) const
{
	for (const int32 ItemSlotIndex : ChangedItemSlots)
	{
		NotifyQuickSlotItemChanged(ItemSlotIndex);
	}
}
