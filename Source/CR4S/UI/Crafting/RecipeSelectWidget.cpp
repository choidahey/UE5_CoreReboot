#include "RecipeSelectWidget.h"

#include "CR4S.h"
#include "ButtonWidget/RecipeSelectButtonWidget.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/RecipeCategoryData.h"

void URecipeSelectWidget::InitWidget() const
{
	if (IsValid(ButtonContainer))
	{
		for (UWidget* Widget : ButtonContainer->GetAllChildren())
		{
			URecipeSelectButtonWidget* RecipeSelectButtonWidget = Cast<URecipeSelectButtonWidget>(Widget);
			if (IsValid(RecipeSelectButtonWidget))
			{
				RecipeSelectButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void URecipeSelectWidget::OpenWidget(const FRecipeCategoryData& RecipeCategoryData)
{
	CraftingCategoryIcon->SetBrushFromTexture(RecipeCategoryData.Icon);
	CraftingCategoryName->SetText(RecipeCategoryData.Name);

	SetVisibility(ESlateVisibility::Visible);
}

void URecipeSelectWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
