#include "CraftingCategoryButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Crafting/CraftingContainerWidget.h"

void UCraftingCategoryButtonWidget::InitWidget(UCraftingContainerWidget* CraftingContainerWidget)
{
	if (IsValid(Button))
	{
		Button->OnClicked.AddUniqueDynamic(CraftingContainerWidget, &UCraftingContainerWidget::OpenRecipeSelectWidget);
	}

	if (IsValid(Icon))
	{
		Icon->SetBrushFromTexture(IconTexture);
	}
}
