#include "PlayerInventoryComponent.h"

#include "CR4S.h"
#include "Game/SaveGame/InventorySaveGame.h"
#include "Gimmick/Components/InteractionComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "UI/InGame/SurvivalHUD.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
	: InventoryContainerWidgetOrder(20),
	  HeldToolTag(FGameplayTag::EmptyTag)
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxInventorySize = 50;
}

void UPlayerInventoryComponent::InitInventory()
{
	Super::InitInventory();

	QuickSlotInventoryComponent = NewObject<UBaseInventoryComponent>(this);
	QuickSlotInventoryComponent->SetMaxInventorySize(10);
	QuickSlotInventoryComponent->InitInventory();

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

	InventoryContainerWidgetInstance = SurvivalHUD->GetInventoryContainerWidget();

	if (CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		InventoryContainerWidgetInstance->InitWidget(SurvivalHUD, this);
	}

	AddItem(FName("StoneAxe"), 1);
}

FAddItemResult UPlayerInventoryComponent::AddItem(const FName RowName, const int32 Count,
                                                  UBaseInventoryItem* OriginItem)
{
	const FAddItemResult Result = Super::AddItem(RowName, Count, OriginItem);

	if (!Result.bSuccess ||
		Result.RemainingCount <= 0)
	{
		return Result;
	}

	return QuickSlotInventoryComponent->AddItem(RowName, Result.RemainingCount, OriginItem);
}

int32 UPlayerInventoryComponent::RemoveItemByRowName(const FName RowName, const int32 Count)
{
	int32 RemainingCount = Super::RemoveItemByRowName(RowName, Count);

	if (IsValid(QuickSlotInventoryComponent))
	{
		RemainingCount = QuickSlotInventoryComponent->RemoveItemByRowName(RowName, RemainingCount);
	}

	return RemainingCount;
}

void UPlayerInventoryComponent::RemoveAllItemByRowName(const FName RowName)
{
	Super::RemoveAllItemByRowName(RowName);

	if (IsValid(QuickSlotInventoryComponent))
	{
		QuickSlotInventoryComponent->RemoveAllItemByRowName(RowName);
	}
}

int32 UPlayerInventoryComponent::GetItemCountByRowName(const FName RowName) const
{
	int32 Count = Super::GetItemCountByRowName(RowName);

	if (IsValid(QuickSlotInventoryComponent))
	{
		Count += QuickSlotInventoryComponent->GetItemCountByRowName(RowName);
	}

	return Count;
}

UPlanterBoxInventoryWidget* UPlayerInventoryComponent::GetPlanterBoxInventoryWidget() const
{
	if (IsValid(InventoryContainerWidgetInstance))
	{
		return InventoryContainerWidgetInstance->GetPlanterBoxInventoryWidget();
	}

	return nullptr;
}

URobotInventoryWidget* UPlayerInventoryComponent::GetRobotInventoryWidget() const
{
	if (IsValid(InventoryContainerWidgetInstance))
	{
		return InventoryContainerWidgetInstance->GetRobotInventoryWidget();
	}

	return nullptr;
}

bool UPlayerInventoryComponent::PrepareOpenInventory(UInteractionComponent* InteractionComponent) const
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(OwnerActor)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		return false;
	}

	InteractionComponent = OwnerActor->FindComponentByClass<UInteractionComponent>();
	if (IsValid(InteractionComponent))
	{
		InteractionComponent->StopDetectProcess();
	}

	return true;
}

void UPlayerInventoryComponent::OpenPlayerInventoryWidget(const int32 CraftingDifficulty) const
{
	if (!PrepareOpenInventory())
	{
		return;
	}

	InventoryContainerWidgetInstance->OpenPlayerInventoryWidget(true, CraftingDifficulty);

	if (OnInventoryOpen.IsBound())
	{
		OnInventoryOpen.Broadcast();
	}
}

void UPlayerInventoryComponent::OpenOtherInventoryWidget(const EOpenWidgetType InventoryType,
                                                         UBaseInventoryComponent* InventoryComponent) const
{
	if (!PrepareOpenInventory())
	{
		return;
	}

	InventoryContainerWidgetInstance->OpenOtherInventoryWidget(InventoryType, InventoryComponent);
}

void UPlayerInventoryComponent::CloseInventoryWidget() const
{
	UInteractionComponent* InteractionComponent = nullptr;
	if (!PrepareOpenInventory(InteractionComponent))
	{
		return;
	}

	if (OnInventoryClose.IsBound())
	{
		OnInventoryClose.Broadcast();
	}

	InventoryContainerWidgetInstance->CloseInventoryWidget();

	InteractionComponent = OwnerActor->FindComponentByClass<UInteractionComponent>();
	if (IsValid(InteractionComponent))
	{
		InteractionComponent->StartDetectProcess();
	}
}

void UPlayerInventoryComponent::ToggleQuickSlotBarWidget()
{
	if (CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		InventoryContainerWidgetInstance->ToggleQuickSlotBar();
	}
}

void UPlayerInventoryComponent::UseItem(const int32 Index) const
{
	if (IsValid(QuickSlotInventoryComponent))
	{
		UBaseInventoryItem* Item = QuickSlotInventoryComponent->GetInventoryItemByIndex(Index);
		if (IsValid(Item))
		{
			Item->UseItem(Index);
		}
	}
}

void UPlayerInventoryComponent::GetPlayerInventorySaveGame(FInventorySaveGame& OutPlayerInventorySaveGame,
                                                           FInventorySaveGame& OutQuickSlotSaveGame)
{
	OutPlayerInventorySaveGame = Super::GetInventorySaveGame();

	if (IsValid(QuickSlotInventoryComponent))
	{
		OutQuickSlotSaveGame = QuickSlotInventoryComponent->GetInventorySaveGame();
	}
}

void UPlayerInventoryComponent::LoadPlayerInventorySaveGame(const FInventorySaveGame& PlayerInventorySaveGame,
                                                            const FInventorySaveGame& QuickSlotSaveGame)
{
	Super::LoadInventorySaveGame(PlayerInventorySaveGame);

	if (IsValid(QuickSlotInventoryComponent))
	{
		QuickSlotInventoryComponent->LoadInventorySaveGame(QuickSlotSaveGame);
	}
}
