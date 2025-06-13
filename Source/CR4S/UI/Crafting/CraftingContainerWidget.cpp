#include "CraftingContainerWidget.h"

#include "CR4S.h"
#include "CraftingCategorySelectWidget.h"
#include "CraftingWidget.h"
#include "RecipeSelectWidget.h"
#include "Gimmick/Data/ItemRecipeData.h"
#include "Gimmick/Data/RecipeCategoryData.h"

void UCraftingContainerWidget::InitWidget(UPlayerInventoryComponent* PlayerInventoryComponent)
{
	if (IsValid(CraftingCategorySelectWidget))
	{
		CraftingCategorySelectWidget->InitWidget(this);
	}

	CloseCraftingWidget();

	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->InitWidget(CraftingWidget);

		CloseRecipeSelectWidget();
	}

	if (IsValid(CraftingWidget))
	{
		CraftingWidget->InitWidget(PlayerInventoryComponent);
	}
}

void UCraftingContainerWidget::UpdateWidget(const int32 NewCraftingDifficulty)
{
	CraftingDifficulty = NewCraftingDifficulty;

	CR4S_Log(LogCraftingUI, Warning, TEXT("Crafting Difficulty: %d"), NewCraftingDifficulty);

	CloseCraftingWidget();

	CloseRecipeSelectWidget();
}

void UCraftingContainerWidget::OpenRecipeSelectWidget(const FRecipeCategoryData& RecipeCategoryData) const
{
	if (IsValid(RecipeSelectWidget) && IsValid(ItemRecipeData))
	{
		TArray<const FItemRecipeData*> RecipeItems;
		for (auto& Pair : ItemRecipeData->GetRowMap())
		{
			if (const FItemRecipeData* Data = reinterpret_cast<FItemRecipeData*>(Pair.Value))
			{
				if (Data->RecipeTag == RecipeCategoryData.RecipeTag &&
					Data->CraftingDifficulty <= CraftingDifficulty)
				{
					RecipeItems.AddUnique(Data);
				}
			}
		}
		
		RecipeSelectWidget->OpenWidget(RecipeCategoryData, RecipeItems);
	}

	CloseCraftingWidget();
}

void UCraftingContainerWidget::CloseRecipeSelectWidget() const
{
	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->CloseWidget();
	}
}

void UCraftingContainerWidget::OpenCraftingWidget(const FRecipeSelectData& NewRecipeSelectData) const
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->OpenWidget(NewRecipeSelectData);
	}
}

void UCraftingContainerWidget::CloseCraftingWidget() const
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->CloseWidget();
	}
}

