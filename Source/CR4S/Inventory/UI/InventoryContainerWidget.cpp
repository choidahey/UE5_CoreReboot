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
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlanterBoxInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CompostBinWidget)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);

	PlayerInventoryWidget->InitWidget(SurvivalHUD, true);
	PlayerInventoryWidget->ConnectInventoryComponent(PlayerInventoryComponent, true, true);
	QuickSlotBarWidget->InitWidget(InPlayerInventoryComponent);
	StorageInventoryWidget->InitWidget(SurvivalHUD, true);
	PlanterBoxInventoryWidget->InitWidget(SurvivalHUD, false);

	InitToggleWidget(PlayerInventoryWidget);
	InitToggleWidget(StorageInventoryWidget);
	InitToggleWidget(PlanterBoxInventoryWidget);
	InitToggleWidget(CompostBinWidget);
}

void UInventoryContainerWidget::OpenPlayerInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		bIsOpen ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(BackgroundBorder)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
	SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly, this);
	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);

	bIsOpen = true;
}

void UInventoryContainerWidget::OpenOtherInventoryWidget(const EInventoryType InventoryType,
                                                         UBaseInventoryComponent* InventoryComponent)
{
	OpenPlayerInventoryWidget();

	OtherInventoryComponent = InventoryComponent;

	bool bCanDrag = false;
	bool bCanDrop = true;
	UUserWidget* TargetWidget = GetTargetInventoryWidget(InventoryType, bCanDrag, bCanDrop);

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(TargetWidget)))
	{
		return;
	}

	UBaseInventoryWidget* TargetInventoryWidget = Cast<UBaseInventoryWidget>(TargetWidget);
	if (IsValid(TargetInventoryWidget))
	{
		TargetInventoryWidget->ConnectInventoryComponent(InventoryComponent, bCanDrag, bCanDrop);
	}

	SurvivalHUD->ToggleWidget(TargetWidget);
	OpenOtherWidget = TargetWidget;
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

	if (IsValid(OtherInventoryComponent) && OtherInventoryComponent->OnOccupiedSlotsChanged.IsBound())
	{
		OtherInventoryComponent->OnOccupiedSlotsChanged.Clear();
	}

	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);
	SurvivalHUD->ToggleWidget(OpenOtherWidget);

	UBaseInventoryWidget* OtherInventoryWidget = Cast<UBaseInventoryWidget>(OpenOtherWidget);
	if (IsValid(OtherInventoryWidget))
	{
		OtherInventoryWidget->UnBoundWidgetDelegate();
	}

	OpenOtherWidget = nullptr;

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

UUserWidget* UInventoryContainerWidget::GetTargetInventoryWidget(
	const EInventoryType InventoryType, bool& bCanDrag, bool& bCanDrop) const
{
	UUserWidget* TargetWidget = nullptr;
	UBaseInventoryWidget* TargetInventoryWidget;

	switch (InventoryType)
	{
	case EInventoryType::Player:
	case EInventoryType::Greenhouse:
		return nullptr;
	case EInventoryType::Storage:
		{
			TargetWidget = StorageInventoryWidget;
			bCanDrag = true;
			TargetInventoryWidget = Cast<UBaseInventoryWidget>(TargetWidget);
			if (IsValid(TargetInventoryWidget))
			{
				TargetInventoryWidget->SetCanSort(true);
			}
			break;
		}
	case EInventoryType::ItemPouch:
		{
			TargetWidget = StorageInventoryWidget;
			bCanDrag = true;
			bCanDrop = false;
			TargetInventoryWidget = Cast<UBaseInventoryWidget>(TargetWidget);
			if (IsValid(TargetInventoryWidget))
			{
				TargetInventoryWidget->SetCanSort(false);
			}
			break;
		}
	case EInventoryType::PlantBox:
		TargetWidget = PlanterBoxInventoryWidget;
		break;
	case EInventoryType::CompostBin:
		TargetWidget = CompostBinWidget;
		break;
	}

	return TargetWidget;
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

void UInventoryContainerWidget::MoveItemToInventory(UBaseItemSlotWidget* ItemSlot, const bool bTargetIsPlayer) const
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(OtherInventoryComponent)))
	{
		return;
	}

	UBaseInventoryItem* Item = ItemSlot->GetCurrentItem();

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(Item)))
	{
		return;
	}

	const FName RowName = Item->GetInventoryItemData()->RowName;

	UBaseInventoryComponent* ToInventoryComponent = bTargetIsPlayer
		                                                ? PlayerInventoryComponent
		                                                : OtherInventoryComponent;

	const FAddItemResult Result = ToInventoryComponent->AddItem(RowName, Item->GetCurrentStackCount());

	Item->SetCurrentStackCount(Result.RemainingCount);
	ItemSlot->SetItem(Item);
}
