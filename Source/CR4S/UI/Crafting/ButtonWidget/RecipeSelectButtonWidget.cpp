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
	
	if (IsValid(RecipeButton))
	{
		RecipeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenCraftingWidget);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URecipeSelectButtonWidget::OpenCraftingWidget()
{
	if (IsValid(CraftingWidget))
	{
		
	}
	
	if (IsValid(CraftingContainerWidget))
	{
		CraftingContainerWidget->OpenCraftingWidget(RecipeSelectData);
	}
}

void URecipeSelectButtonWidget::SetRecipeData(const FRecipeSelectData& NewRecipeData)
{
	RecipeSelectData = NewRecipeData;
	
	if (IsValid(RecipeIcon))
	{
		RecipeIcon->SetBrushFromTexture(NewRecipeData.Icon, true);
	}

	if (IsValid(RecipeName))
	{
		RecipeName->SetText(NewRecipeData.Name);
	}
}
