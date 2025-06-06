#include "CraftingCategoryButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Crafting/CraftingContainerWidget.h"

UCraftingCategoryButtonWidget::UCraftingCategoryButtonWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CraftingContainerWidget = nullptr;
	RecipeCategoryData = FRecipeCategoryData();
}

void UCraftingCategoryButtonWidget::InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget, const FRecipeCategoryData& NewRecipeCategoryData)
{
	CraftingContainerWidget = NewCraftingContainerWidget;
	RecipeCategoryData = NewRecipeCategoryData;
	
	if (IsValid(CraftingCategoryButton))
	{
		CraftingCategoryButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenRecipeSelectWidget);
	}

	if (IsValid(CraftingCategoryIcon))
	{
		CraftingCategoryIcon->SetBrushFromTexture(NewRecipeCategoryData.Icon);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UCraftingCategoryButtonWidget::OpenRecipeSelectWidget()
{
	if (IsValid(CraftingContainerWidget))
	{
		CraftingContainerWidget->OpenRecipeSelectWidget(RecipeCategoryData);
	}
}
