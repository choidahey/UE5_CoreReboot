#include "RecipeSelectButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/Crafting/CraftingContainerWidget.h"
#include "UI/Crafting/CraftingWidget.h"

void URecipeSelectButtonWidget::InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget)
{
	CraftingContainerWidget = NewCraftingContainerWidget;
	if (IsValid(CraftingContainerWidget))
	{
		CraftingWidget = CraftingContainerWidget->GetCraftingWidget();
	}
	
	if (IsValid(Button))
	{
		Button->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenCraftingWidget);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URecipeSelectButtonWidget::OpenCraftingWidget()
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->InitWidget();
	}
	
	if (IsValid(CraftingContainerWidget))
	{
		CraftingContainerWidget->OpenCraftingWidget();
	}
}

void URecipeSelectButtonWidget::SetRecipeData(const FRecipeSelectData& RecipeData) const
{
	if (IsValid(Icon))
	{
		Icon->SetBrushFromTexture(RecipeData.IconTexture);
	}

	if (IsValid(RecipeName))
	{
		RecipeName->SetText(RecipeData.RecipeName);
	}
}
