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
		BackgroundIcon->SetBrushFromTexture(BackgroundIconTexture);
	}

	PartsTypeName = FText();
	
	UpdateRobotWorkshopMode(ERobotWorkshopMode::Create);

	bCanRightClick = false;
}

void URobotPartsItemSlotWidget::InitSlotWidgetData(UBaseInventoryWidget* NewInventoryWidget,
                                                   UBaseInventoryItem* NewItem)
{
	Super::InitSlotWidgetData(NewInventoryWidget, NewItem);

	RobotInventoryWidget = Cast<URobotInventoryWidget>(NewInventoryWidget);
	
	if (IsValid(SelectButton))
	{
		SelectButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenRecipeSelectWidget);
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
		UTexture2D* Icon = Cast<UTexture2D>(IconImage->GetBrush().GetResourceObject());
		RobotInventoryWidget->OpenRecipeSelectWidget(Icon, PartsTypeName, PartsTypeTag);
	}
}

void URobotPartsItemSlotWidget::UpdateRobotWorkshopMode(const ERobotWorkshopMode RobotWorkshopMode) const
{
	if (IsValid(SelectButton))
	{
		const bool bIsCreateMode = RobotWorkshopMode == ERobotWorkshopMode::Create;
		SelectButton->SetVisibility(bIsCreateMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
