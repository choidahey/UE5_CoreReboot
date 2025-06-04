#include "CraftingCategorySelectWidget.h"

#include "ButtonWidget/CraftingCategoryButtonWidget.h"
#include "Components/PanelWidget.h"

void UCraftingCategorySelectWidget::InitWidget(UCraftingContainerWidget* CraftingContainerWidget)
{
	if (IsValid(ButtonContainer))
	{
		for (UWidget* Widget : ButtonContainer->GetAllChildren())
		{
			UCraftingCategoryButtonWidget* CraftingCategoryButtonWidget = Cast<UCraftingCategoryButtonWidget>(Widget);
			if (IsValid(CraftingCategoryButtonWidget))
			{
				CraftingCategoryButtonWidget->InitWidget(CraftingContainerWidget);
				CraftingCategoryButtonWidgets.AddUnique(CraftingCategoryButtonWidget);
			}
		}
	}
}
