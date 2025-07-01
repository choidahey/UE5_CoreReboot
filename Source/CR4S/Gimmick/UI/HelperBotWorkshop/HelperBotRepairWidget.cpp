#include "HelperBotRepairWidget.h"

#include "HelperBotManagementWidget.h"
#include "Components/Button.h"
#include "UI/Crafting/IngredientsWidget.h"

void UHelperBotRepairWidget::InitWidget(UHelperBotManagementWidget* NewHelperBotManagementWidget)
{
	HelperBotManagementWidget = NewHelperBotManagementWidget;
	
	if (IsValid(IngredientsContainer))
	{
		for (UWidget* Widget : IngredientsContainer->GetAllChildren())
		{
			UIngredientsWidget* IngredientsWidget = Cast<UIngredientsWidget>(Widget);
			if (IsValid(IngredientsWidget))
			{
				IngredientsWidgets.AddUnique(IngredientsWidget);
			}
		}
	}

	// UpdateIngredients();
	
	if (IsValid(RepairButton))
	{
		RepairButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenWidget);
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::CloseWidget);
	}
}

void UHelperBotRepairWidget::UpdateIngredients()
{
	for (UIngredientsWidget* IngredientsWidget : IngredientsWidgets)
	{
		if (IsValid(IngredientsWidget))
		{
			IngredientsWidget->InitWidget(FIngredientData());
		}
	}
}

void UHelperBotRepairWidget::OpenWidget()
{
	SetVisibility(ESlateVisibility::Visible);

	if (IsValid(HelperBotManagementWidget))
	{
		HelperBotManagementWidget->CloseWidget();
	}
}

void UHelperBotRepairWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Hidden);

	if (IsValid(HelperBotManagementWidget))
	{
		HelperBotManagementWidget->OpenWidget();
	}
}
