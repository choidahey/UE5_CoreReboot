#include "CraftingCategorySelectWidget.h"

#include "ButtonWidget/CraftingCategoryButtonWidget.h"
#include "Components/PanelWidget.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Gimmick/Data/RecipeCategoryData.h"

void UCraftingCategorySelectWidget::InitWidget(UCraftingContainerWidget* CraftingContainerWidget)
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
		if (IsValid(CategoryButtonWidgetClass))
		{
			UCraftingCategoryButtonWidget* CraftingCategoryButtonWidget
				= CreateWidget<UCraftingCategoryButtonWidget>(GetWorld(), CategoryButtonWidgetClass);
			if (IsValid(CraftingCategoryButtonWidget))
			{
				ButtonContainer->AddChild(CraftingCategoryButtonWidget);
				const FRecipeCategoryData* RecipeCategoryData
					= RecipeCategoryDataTable->FindRow<FRecipeCategoryData>(RowName,TEXT("RecipeCategoryData"));

				if (RecipeCategoryData)
				{
					CraftingCategoryButtonWidget->InitWidget(CraftingContainerWidget, *RecipeCategoryData);

					if (RecipeCategoryData->RecipeTag.MatchesTag(CookingRecipeTag))
					{
						CookingCategoryWidgets.Add(CraftingCategoryButtonWidget);
					}
					else
					{
						CraftingCategoryWidgets.Add(CraftingCategoryButtonWidget);
					}
				}
			}
		}
	}
}

void UCraftingCategorySelectWidget::UpdateCategories(const int32 NewCraftingDifficulty)
{
	const bool IsCookingRecipe = NewCraftingDifficulty == 10;
	UpdateWidget(CraftingCategoryWidgets, !IsCookingRecipe);
	UpdateWidget(CookingCategoryWidgets, IsCookingRecipe);
}

void UCraftingCategorySelectWidget::UpdateWidget(TArray<TObjectPtr<UCraftingCategoryButtonWidget>>& CategoryWidgets,
                                                 const bool IsCookingRecipe)
{
	for (UCraftingCategoryButtonWidget* CraftingCategoryButtonWidget : CategoryWidgets)
	{
		if (IsValid(CraftingCategoryButtonWidget))
		{
			CraftingCategoryButtonWidget->SetVisibility(IsCookingRecipe
				                                            ? ESlateVisibility::Visible
				                                            : ESlateVisibility::Collapsed);
		}
	}
}
