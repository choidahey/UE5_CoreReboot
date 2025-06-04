#include "RecipeSelectWidget.h"

#include "ButtonWidget/RecipeSelectButtonWidget.h"
#include "Components/PanelWidget.h"

void URecipeSelectWidget::InitWidget()
{
	if (IsValid(ButtonContainer))
	{
		for (UWidget* Widget : ButtonContainer->GetAllChildren())
		{
			URecipeSelectButtonWidget* RecipeSelectButtonWidget = Cast<URecipeSelectButtonWidget>(Widget);
			if (IsValid(RecipeSelectButtonWidget))
			{
				// RecipeSelectButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void URecipeSelectWidget::OpenWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void URecipeSelectWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
