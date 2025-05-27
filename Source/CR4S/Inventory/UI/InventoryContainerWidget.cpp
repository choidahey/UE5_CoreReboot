#include "InventoryContainerWidget.h"

#include "BaseInventoryWidget.h"
#include "CR4S.h"
#include "InventoryType.h"
#include "Components/Border.h"
#include "UI/InGame/SurvivalHUD.h"

void UInventoryContainerWidget::InitWidget(ASurvivalHUD* InSurvivalHUD,
                                           UPlayerInventoryComponent* InPlayerInventoryComponent)
{
	bIsOpen = false;
	SurvivalHUD = InSurvivalHUD;
	PlayerInventoryComponent = InPlayerInventoryComponent;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);

	if (PlayerInventoryWidget->GetVisibility() != ESlateVisibility::Collapsed)
	{
		SurvivalHUD->ToggleWidget(PlayerInventoryWidget);
	}

	if (ItemPouchWidget->GetVisibility() != ESlateVisibility::Collapsed)
	{
		SurvivalHUD->ToggleWidget(ItemPouchWidget);
	}
}

void UInventoryContainerWidget::ToggleInventoryWidget(const EInventoryType InventoryType)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(bIsOpen ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	SurvivalHUD->SetInputMode(bIsOpen ? ESurvivalInputMode::GameOnly : ESurvivalInputMode::UIOnly);

	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);

	if (InventoryType != EInventoryType::Player)
	{
		if (CR4S_VALIDATE(LogInventoryUI, InventoryWidgets.Contains(InventoryType)))
		{
			SurvivalHUD->ToggleWidget(InventoryWidgets[InventoryType]);
		}
	}

	bIsOpen = !bIsOpen;
}
