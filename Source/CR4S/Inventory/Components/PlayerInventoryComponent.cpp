#include "PlayerInventoryComponent.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractionComponent.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "UI/InGame/SurvivalHUD.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
	: InventoryContainerWidgetOrder(0),
	  HeldToolTag(FGameplayTag())
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxInventorySize = 50;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

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

	InventoryContainerWidgetInstance = SurvivalHUD->CreateAndAddWidget(InventoryContainerWidgetClass,
	                                                                   InventoryContainerWidgetOrder,
	                                                                   ESlateVisibility::Visible);

	if (CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		InventoryContainerWidgetInstance->InitWidget(SurvivalHUD, this);
	}

	AddItem(FName("StoneAxe"), 1);
}

FAddItemResult UPlayerInventoryComponent::AddItem(const FName RowName, const int32 Count)
{
	const FAddItemResult Result = Super::AddItem(RowName, Count);

	if (!Result.bSuccess ||
		Result.RemainingCount <= 0)
	{
		return Result;
	}

	return QuickSlotInventoryComponent->AddItem(RowName, Result.RemainingCount);
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
}

void UPlayerInventoryComponent::OpenOtherInventoryWidget(const EInventoryType InventoryType,
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

	InventoryContainerWidgetInstance->CloseInventoryWidget();

	InteractionComponent = OwnerActor->FindComponentByClass<UInteractionComponent>();
	if (IsValid(InteractionComponent))
	{
		InteractionComponent->StartDetectProcess();
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
