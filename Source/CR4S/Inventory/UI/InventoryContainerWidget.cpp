#include "InventoryContainerWidget.h"

#include "CR4S.h"
#include "InventoryType.h"
#include "Components/Border.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "InventoryWidget/BaseInventoryWidget.h"
#include "InventoryWidget/QuickSlotBarWidget.h"
#include "InventoryWidget/StorageInventoryWidget.h"
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

	PlayerInventoryWidget->InitWidget(SurvivalHUD, this);
	PlayerInventoryWidget->ConnectInventoryComponent(PlayerInventoryComponent, true, true);
	QuickSlotBarWidget->InitWidget(InPlayerInventoryComponent);
	
	StorageInventoryWidget->InitWidget(SurvivalHUD, this);
	PlantBoxInventoryWidget->InitWidget(SurvivalHUD, this);
	CompostBinInventoryWidget->InitWidget(SurvivalHUD, this);

	InitToggleWidget(PlayerInventoryWidget);
	InitToggleWidget(StorageInventoryWidget);
	InitToggleWidget(PlantBoxInventoryWidget);
	InitToggleWidget(CompostBinInventoryWidget);
}

void UInventoryContainerWidget::OpenPlayerInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)))
	{
		return;
	}

	if (CR4S_VALIDATE(LogInventoryUI, bIsOpen))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
	SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly);
	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);

	OpenInventoryWidgets.AddUnique(PlayerInventoryWidget);

	bIsOpen = true;
}

void UInventoryContainerWidget::OpenOtherInventoryWidget(const EInventoryType InventoryType,
                                                         UBaseInventoryComponent* InventoryComponent)
{
	OpenPlayerInventoryWidget();

	UBaseInventoryWidget* TargetInventoryWidget = nullptr;

	bool bCanDrag = false;
	bool bCanDrop = true;
	
	switch (InventoryType)
	{
	case EInventoryType::Player:
	case EInventoryType::Greenhouse:
		return;
	case EInventoryType::Storage:
		TargetInventoryWidget = StorageInventoryWidget;
		bCanDrag = true;
		break;
	case EInventoryType::PlantBox:
		TargetInventoryWidget = PlantBoxInventoryWidget;
		break;
	case EInventoryType::CompostBin:
		TargetInventoryWidget = CompostBinInventoryWidget;
		break;
	}
	
	TargetInventoryWidget->ConnectInventoryComponent(InventoryComponent, bCanDrag, bCanDrop);

	SurvivalHUD->ToggleWidget(TargetInventoryWidget);
	OpenInventoryWidgets.AddUnique(TargetInventoryWidget);
}

void UInventoryContainerWidget::CloseInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);

	for (UBaseInventoryWidget* OpenInventoryWidget : OpenInventoryWidgets)
	{
		if (IsValid(OpenInventoryWidget))
		{
			SurvivalHUD->ToggleWidget(OpenInventoryWidget);

			if (OpenInventoryWidget != PlayerInventoryWidget)
			{
				OpenInventoryWidget->UnBoundOnItemSlotChanged();
			}
		}
	}

	OpenInventoryWidgets.Empty();

	SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);

	bIsOpen = false;
}

void UInventoryContainerWidget::InitToggleWidget(UBaseInventoryWidget* InventoryWidget) const
{
	if (CR4S_VALIDATE(LogInventoryUI, InventoryWidget->GetVisibility() != ESlateVisibility::Collapsed))
	{
		SurvivalHUD->ToggleWidget(InventoryWidget);
	}
}
