#include "PlayerInventoryComponent.h"

#include "CR4S.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "UI/InGame/SurvivalHUD.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
	: InventoryContainerWidgetOrder(0)
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

void UPlayerInventoryComponent::OpenPlayerInventoryWidget(const int32 CraftingDifficulty)
{
	if (!CR4S_VALIDATE(LogInventory, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventory, IsValid(InventoryContainerWidgetInstance)))
	{
		return;
	}

	InventoryContainerWidgetInstance->OpenPlayerInventoryWidget(true, CraftingDifficulty);
}

void UPlayerInventoryComponent::OpenOtherInventoryWidget(const EInventoryType InventoryType, UBaseInventoryComponent* InventoryComponent) const
{
	InventoryContainerWidgetInstance->OpenOtherInventoryWidget(InventoryType, InventoryComponent);
}

void UPlayerInventoryComponent::CloseInventoryWidget() const
{
	if (IsValid(InventoryContainerWidgetInstance))
	{
		InventoryContainerWidgetInstance->CloseInventoryWidget();
	}
}
