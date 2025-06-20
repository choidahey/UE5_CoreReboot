#include "InventoryContainerWidget.h"

#include "CR4S.h"
#include "Components/Border.h"
#include "Gimmick/UI/CompostBinWidget.h"
#include "Gimmick/UI/HelperBotWorkshop/HelperBotWorkshopWidget.h"
#include "Inventory/OpenWidgetType.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "InventoryWidget/BaseInventoryWidget.h"
#include "InventoryWidget/PlanterBoxInventoryWidget.h"
#include "InventoryWidget/RobotInventoryWidget.h"
#include "InventoryWidget/StorageInventoryWidget.h"
#include "UI/Crafting/CraftingContainerWidget.h"
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
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(InputGuideContainer)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(QuickSlotBarWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(StorageInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(PlanterBoxInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(CompostBinWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(RobotWorkshopWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(HelperBotWorkshopWidget)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);

	PlayerInventoryWidget->InitWidget(SurvivalHUD, true);
	PlayerInventoryWidget->ConnectInventoryComponent(PlayerInventoryComponent);

	QuickSlotBarWidget->InitWidget(SurvivalHUD, false);
	if (IsValid(PlayerInventoryComponent))
	{
		QuickSlotBarWidget->ConnectInventoryComponent(PlayerInventoryComponent->GetQuickSlotInventoryComponent());
	}

	StorageInventoryWidget->InitWidget(SurvivalHUD, true);
	PlanterBoxInventoryWidget->InitWidget(SurvivalHUD, false);
	CraftingContainerWidget->InitWidget(PlayerInventoryComponent);
	RobotWorkshopWidget->InitWidget(SurvivalHUD, false);
	HelperBotWorkshopWidget->InitWidget(PlayerInventoryComponent);

	InitToggleWidget(PlayerInventoryWidget);
	InputGuideContainer->SetVisibility(ESlateVisibility::Collapsed);
	InitToggleWidget(StorageInventoryWidget);
	InitToggleWidget(PlanterBoxInventoryWidget);
	InitToggleWidget(CompostBinWidget);
	InitToggleWidget(CraftingContainerWidget);
	InitToggleWidget(RobotWorkshopWidget);
	InitToggleWidget(HelperBotWorkshopWidget);
}

void UInventoryContainerWidget::OpenPlayerInventoryWidget(const bool bOpenCraftingWidget,
                                                          const int32 CraftingDifficulty)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		bIsOpen ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(BackgroundBorder)))
	{
		return;
	}

	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);
	InputGuideContainer->SetVisibility(ESlateVisibility::Visible);

	if (bOpenCraftingWidget)
	{
		OpenCraftingWidget(CraftingDifficulty);
	}

	bIsOpen = true;

	if (IsValid(PlayerInventoryComponent))
	{
		ChangeWidgetOrder(PlayerInventoryComponent->GetInventoryContainerWidgetOrder());
	}

	SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly, this);
}

void UInventoryContainerWidget::OpenOtherInventoryWidget(const EOpenWidgetType InventoryType,
                                                         UBaseInventoryComponent* InventoryComponent)
{
	OpenPlayerInventoryWidget();

	OtherInventoryComponent = InventoryComponent;

	UUserWidget* TargetWidget = GetTargetInventoryWidget(InventoryType);

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(TargetWidget)))
	{
		return;
	}

	UBaseInventoryWidget* TargetInventoryWidget = Cast<UBaseInventoryWidget>(TargetWidget);
	if (IsValid(TargetInventoryWidget))
	{
		TargetInventoryWidget->ConnectInventoryComponent(InventoryComponent);
	}

	SurvivalHUD->ToggleWidget(TargetWidget);
	OpenOtherWidget = TargetWidget;
}

void UInventoryContainerWidget::OpenCraftingWidget(const int32 CraftingDifficulty)
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CraftingContainerWidget)))
	{
		return;
	}

	CraftingContainerWidget->UpdateWidget(CraftingDifficulty);

	SurvivalHUD->ToggleWidget(CraftingContainerWidget);
	OpenOtherWidget = CraftingContainerWidget;
}

void UInventoryContainerWidget::ToggleQuickSlotBar() const
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!IsValid(QuickSlotBarWidget))
	{
		return;
	}

	SurvivalHUD->ToggleWidget(QuickSlotBarWidget);
}

void UInventoryContainerWidget::CloseInventoryWidget()
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(SurvivalHUD)) ||
		!IsValid(BackgroundBorder) ||
		!IsValid(PlayerInventoryWidget))
	{
		return;
	}

	bIsOpen = false;

	ChangeWidgetOrder(0);

	if (IsValid(OtherInventoryComponent) && OtherInventoryComponent->OnOccupiedSlotsChange.IsBound())
	{
		OtherInventoryComponent->OnOccupiedSlotsChange.Clear();
	}

	if (IsValid(QuickSlotBarWidget) && QuickSlotBarWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		ToggleQuickSlotBar();
	}

	SurvivalHUD->ToggleWidget(PlayerInventoryWidget);

	if (IsValid(InputGuideContainer))
	{
		InputGuideContainer->SetVisibility(ESlateVisibility::Collapsed);
	}

	SurvivalHUD->ToggleWidget(OpenOtherWidget);

	UBaseInventoryWidget* OtherInventoryWidget = Cast<UBaseInventoryWidget>(OpenOtherWidget);
	if (IsValid(OtherInventoryWidget))
	{
		OtherInventoryWidget->UnBoundWidgetDelegate();
	}

	OpenOtherWidget = nullptr;
	OtherInventoryComponent = nullptr;

	BackgroundBorder->SetVisibility(ESlateVisibility::Collapsed);

	SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
}

void UInventoryContainerWidget::ChangeWidgetOrder(const int32 NewOrder)
{
	RemoveFromParent();
	AddToViewport(NewOrder);
}

void UInventoryContainerWidget::InitToggleWidget(UUserWidget* Widget) const
{
	if (CR4S_VALIDATE(LogInventoryUI, Widget->GetVisibility() != ESlateVisibility::Collapsed))
	{
		SurvivalHUD->ToggleWidget(Widget);
	}
}

UUserWidget* UInventoryContainerWidget::GetTargetInventoryWidget(
	const EOpenWidgetType OpenWidgetType) const
{
	UUserWidget* TargetWidget = nullptr;

	switch (OpenWidgetType)
	{
	case EOpenWidgetType::Player:
		return nullptr;
	case EOpenWidgetType::Storage:
		{
			if (IsValid(StorageInventoryWidget))
			{
				StorageInventoryWidget->SetCanSort(true);
				StorageInventoryWidget->SetCanDrop(true);
			}
			TargetWidget = StorageInventoryWidget;
			break;
		}
	case EOpenWidgetType::ItemPouch:
		{
			if (IsValid(StorageInventoryWidget))
			{
				StorageInventoryWidget->SetCanSort(false);
				StorageInventoryWidget->SetCanDrop(false);
			}
			TargetWidget = StorageInventoryWidget;
			break;
		}
	case EOpenWidgetType::PlantBox:
		TargetWidget = PlanterBoxInventoryWidget;
		break;
	case EOpenWidgetType::CompostBin:
		TargetWidget = CompostBinWidget;
		break;
	case EOpenWidgetType::RobotWorkshop:
		TargetWidget = RobotWorkshopWidget;
		break;
	case EOpenWidgetType::HelperBotWorkshop:
		TargetWidget = HelperBotWorkshopWidget;
		break;
	}

	return TargetWidget;
}

FReply UInventoryContainerWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::RightBracket && PlayerInventoryComponent)
	{
		PlayerInventoryComponent->CloseInventoryWidget();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UInventoryContainerWidget::MoveItemToInventory(const UBaseItemSlotWidget* ItemSlot,
                                                    const bool bTargetIsPlayer) const
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(OtherInventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(ItemSlot)) ||
		!CanMoveItem(bTargetIsPlayer))
	{
		return;
	}

	UBaseInventoryItem* Item = ItemSlot->GetCurrentItem();
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(Item)))
	{
		return;
	}

	const FName RowName = Item->GetInventoryItemData()->RowName;
	UBaseInventoryComponent* FromInventoryComponent = bTargetIsPlayer
		                                                  ? OtherInventoryComponent
		                                                  : PlayerInventoryComponent;
	UBaseInventoryComponent* ToInventoryComponent = bTargetIsPlayer
		                                                ? PlayerInventoryComponent
		                                                : OtherInventoryComponent;

	const FAddItemResult Result = ToInventoryComponent->AddItem(RowName, Item->GetCurrentStackCount(), Item);

	FromInventoryComponent->RemoveItemByIndex(ItemSlot->GetSlotIndex(), Result.AddedCount);
}

bool UInventoryContainerWidget::CanMoveItem(const bool bTargetIsPlayer) const
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(PlayerInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(OpenOtherWidget)))
	{
		return false;
	}

	const UBaseInventoryWidget* ToInventoryWidget = bTargetIsPlayer
		                                                ? PlayerInventoryWidget.Get()
		                                                : Cast<UBaseInventoryWidget>(OpenOtherWidget);

	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(ToInventoryWidget)) ||
		!CR4S_VALIDATE(LogInventoryUI, ToInventoryWidget->CanDrop()))
	{
		return false;
	}

	return true;
}
