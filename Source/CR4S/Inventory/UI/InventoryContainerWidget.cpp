#include "InventoryContainerWidget.h"

#include "CR4S.h"
#include "Components/Border.h"
#include "Gimmick/UI/CompostBinWidget.h"
#include "Inventory/InventoryType.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "InventoryWidget/BaseInventoryWidget.h"
#include "InventoryWidget/QuickSlotBarWidget.h"
#include "InventoryWidget/StorageInventoryWidget.h"
#include "UI/InGame/SurvivalHUD.h"

void UInventoryContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}

void UInventoryContainerWidget::InitWidget(ASurvivalHUD* InSurvivalHUD,
                                           UPlayerInventoryComponent* InPlayerInventoryComponent)
{
	bIsOpen = false;
	SurvivalHUD = InSurvivalHUD;
	PlayerInventoryComponent = InPlayerInventoryComponent;

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotBarWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(StorageInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlantBoxInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CompostBinWidget)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);

	PlayerInventoryWidget->InitWidget(SurvivalHUD);
	PlayerInventoryWidget->ConnectInventoryComponent(PlayerInventoryComponent, true, true);
	QuickSlotBarWidget->InitWidget(InPlayerInventoryComponent);

	StorageInventoryWidget->InitWidget(SurvivalHUD);
	PlantBoxInventoryWidget->InitWidget(SurvivalHUD);

	InitToggleWidget(PlayerInventoryWidget);
	InitToggleWidget(StorageInventoryWidget);
	InitToggleWidget(PlantBoxInventoryWidget);
	InitToggleWidget(CompostBinWidget);
}

void UInventoryContainerWidget::OpenPlayerInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		CR4S_VALIDATE(LogInventoryUI, bIsOpen) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(BackgroundBorder)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
	SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly, this);
	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);

	OpenInventoryWidgets.AddUnique(PlayerInventoryWidget);

	bIsOpen = true;
}

void UInventoryContainerWidget::OpenOtherInventoryWidget(const EInventoryType InventoryType,
                                                         UBaseInventoryComponent* InventoryComponent)
{
	OpenPlayerInventoryWidget();

	bool bCanDrag = false;
	bool bCanDrop = true;
	UBaseInventoryWidget* TargetInventoryWidget = GetTargetInventoryWidget(InventoryType, bCanDrag, bCanDrop);

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(TargetInventoryWidget)))
	{
		return;
	}

	TargetInventoryWidget->ConnectInventoryComponent(InventoryComponent, bCanDrag, bCanDrop);

	SurvivalHUD->ToggleWidget(TargetInventoryWidget);
	OpenInventoryWidgets.AddUnique(TargetInventoryWidget);
}

void UInventoryContainerWidget::CloseInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(BackgroundBorder)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryWidget)))
	{
		return;
	}
	
	bIsOpen = false;
	
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

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);
	
	SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
}

void UInventoryContainerWidget::InitToggleWidget(UUserWidget* Widget) const
{
	if (CR4S_VALIDATE(LogInventoryUI, Widget->GetVisibility() != ESlateVisibility::Collapsed))
	{
		SurvivalHUD->ToggleWidget(Widget);
	}
}

UBaseInventoryWidget* UInventoryContainerWidget::GetTargetInventoryWidget(
	const EInventoryType InventoryType, bool& bCanDrag, bool& bCanDrop) const
{
	UBaseInventoryWidget* TargetInventoryWidget = nullptr;

	switch (InventoryType)
	{
	case EInventoryType::Player:
	case EInventoryType::Greenhouse:
		return nullptr;
	case EInventoryType::Storage:
		{
			TargetInventoryWidget = StorageInventoryWidget;
			bCanDrag = true;
			TargetInventoryWidget->SetCanSort(true);
			break;
		}
	case EInventoryType::ItemPouch:
		{
			TargetInventoryWidget = StorageInventoryWidget;
			bCanDrag = true;
			bCanDrop = false;
			TargetInventoryWidget->SetCanSort(false);
			break;
		}
	case EInventoryType::PlantBox:
		TargetInventoryWidget = PlantBoxInventoryWidget;
		TargetInventoryWidget->SetCanSort(false);
		break;
	}

	return TargetInventoryWidget;
}

FReply UInventoryContainerWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Nine)
	{
		CloseInventoryWidget();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
