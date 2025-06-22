#include "CraftingCategorySelectWidget.h"

#include "ButtonWidget/CraftingCategoryButtonWidget.h"
#include "Components/PanelWidget.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Gimmick/Data/RecipeCategoryData.h"

void UCraftingCategorySelectWidget::InitWidget(UCraftingContainerWidget* CraftingContainerWidget) const
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(RecipeCategoryDataTable)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ButtonContainer)))
	{
		return;
	}

	TArray<FName> RowNames = RecipeCategoryDataTable->GetRowNames();

	ButtonContainer->ClearChildren();

	for (const FName RowName : RowNames)
	{
		UCraftingCategoryButtonWidget* CraftingCategoryButtonWidget
			= CreateWidget<UCraftingCategoryButtonWidget>(GetWorld(), CategoryButtonWidgetClass);
		if (IsValid(CraftingCategoryButtonWidget))
		{
			ButtonContainer->AddChild(CraftingCategoryButtonWidget);
			const FRecipeCategoryData* RecipeCategoryData
				= RecipeCategoryDataTable->FindRow<FRecipeCategoryData>(RowName,TEXT("RecipeCategoryData"));

			CraftingCategoryButtonWidget->InitWidget(CraftingContainerWidget, *RecipeCategoryData);
		}
	}
}

void UCraftingCategorySelectWidget::UpdateCategories(const int32 NewCraftingDifficulty)
{
	const bool IsCookingRecipe = NewCraftingDifficulty >= 10; 
	
}
