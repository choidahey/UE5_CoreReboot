#include "RobotPartsItemSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryWidget/RobotInventoryWidget.h"

void URobotPartsItemSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(BackgroundIcon))
	{
		BackgroundIcon->SetBrush(BackgroundIconBrush);
	}
}

void URobotPartsItemSlotWidget::InitSlotWidgetData(UBaseInventoryWidget* NewInventoryWidget,
                                                   UBaseInventoryItem* NewItem)
{
	Super::InitSlotWidgetData(NewInventoryWidget, NewItem);

	RobotInventoryWidget = Cast<URobotInventoryWidget>(NewInventoryWidget);

	if (IsValid(RobotInventoryWidget))
	{
		RobotInventoryWidget->OnRobotWorkshopModeChange.AddUniqueDynamic(this, &ThisClass::UpdateRobotWorkshopMode);
	}

	if (IsValid(SelectButton))
	{
		if (bCanCreateParts)
		{
			SelectButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenRecipeSelectWidget);
		}
		SelectButton->SetIsEnabled(false);
	}
}

bool URobotPartsItemSlotWidget::IsItemAllowedByFilter(UBaseInventoryItem* Item) const
{
	const bool Result = Super::IsItemAllowedByFilter(Item);
	if (Result)
	{
		if (IsValid(Item))
		{
			const FGameplayTagContainer& RobotPartsTag = Item->GetItemTags();
			return RobotPartsTag.HasTag(PartsTypeTag);
		}
	}

	return Result;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URobotPartsItemSlotWidget::OpenRecipeSelectWidget()
{
	if (IsValid(RobotInventoryWidget))
	{
		UTexture2D* Icon = Cast<UTexture2D>(BackgroundIcon->GetBrush().GetResourceObject());
		RobotInventoryWidget->OpenRecipeSelectWidget(Icon, PartsTypeName, PartsTypeTag);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URobotPartsItemSlotWidget::UpdateRobotWorkshopMode(const ERobotWorkshopMode RobotWorkshopMode)
{
	const bool bIsCreateMode = RobotWorkshopMode == ERobotWorkshopMode::Create;

	if (!bCanCreateParts)
	{
		SetVisibility(bIsCreateMode ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	else if (IsValid(SelectButton))
	{
		SelectButton->SetIsEnabled(bIsCreateMode);
	}

	bCanDrag = !bIsCreateMode;
	bCanDrop = !bIsCreateMode;
}
