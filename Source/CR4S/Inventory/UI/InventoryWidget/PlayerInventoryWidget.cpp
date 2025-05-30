#include "PlayerInventoryWidget.h"

#include "Components/Button.h"
#include "Inventory/UI/InventoryContainerWidget.h"

void UPlayerInventoryWidget::InitWidget(ASurvivalHUD* SurvivalHUD,
                                        UInventoryContainerWidget* InventoryContainerWidget)
{
	Super::InitWidget(SurvivalHUD, InventoryContainerWidget);

	if (IsValid(CloseButton))
	{
		CloseButton->OnClicked.AddUniqueDynamic(InventoryContainerWidget,
		                                        &UInventoryContainerWidget::CloseInventoryWidget);
	}
}

void UPlayerInventoryWidget::ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent, const bool bCanDrag, const bool bCanDrop)
{
	Super::ConnectInventoryComponent(NewInventoryComponent, bCanDrag, bCanDrop);

	if (IsValid(SortButton))
	{
		SortButton->OnClicked.AddUniqueDynamic(InventoryComponent, &UBaseInventoryComponent::SortInventoryItems);
	}
}
