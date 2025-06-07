#include "RecipeSelectWidget.h"

#include "CR4S.h"
#include "CraftingContainerWidget.h"
#include "ButtonWidget/RecipeSelectButtonWidget.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/ItemData.h"
#include "Gimmick/Data/RecipeCategoryData.h"

void URecipeSelectWidget::InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget)
{
	CraftingContainerWidget = NewCraftingContainerWidget;

	if (IsValid(ButtonContainer))
	{
		for (UWidget* Widget : ButtonContainer->GetAllChildren())
		{
			URecipeSelectButtonWidget* RecipeSelectButtonWidget = Cast<URecipeSelectButtonWidget>(Widget);
			if (IsValid(RecipeSelectButtonWidget))
			{
				RecipeSelectButtonWidgets.AddUnique(RecipeSelectButtonWidget);
				RecipeSelectButtonWidget->InitWidget(CraftingContainerWidget);
				RecipeSelectButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void URecipeSelectWidget::OpenWidget(const FRecipeCategoryData& RecipeCategoryData)
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(ScrollBox)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingCategoryIcon)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingCategoryName)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemRecipeData)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingContainerWidget)))
	{
		return;
	}

	CraftingCategoryIcon->SetBrushFromTexture(RecipeCategoryData.Icon, true);
	CraftingCategoryName->SetText(RecipeCategoryData.Name);

	TArray<const FItemRecipeData*> RecipeItems;
	for (auto& Pair : ItemRecipeData->GetRowMap())
	{
		if (const FItemRecipeData* Data = reinterpret_cast<FItemRecipeData*>(Pair.Value))
		{
			if (Data->RecipeTag == RecipeCategoryData.RecipeTag &&
				Data->CraftingDifficulty <= CraftingContainerWidget->GetCraftingDifficulty())
			{
				RecipeItems.AddUnique(Data);
			}
		}
	}

	for (int32 Index = 0; Index < RecipeSelectButtonWidgets.Num(); Index++)
	{
		URecipeSelectButtonWidget* RecipeSelectButtonWidget = RecipeSelectButtonWidgets[Index];
		if (!IsValid(RecipeSelectButtonWidget))
		{
			continue;
		}

		if (RecipeItems.IsValidIndex(Index))
		{
			const FItemRecipeData* RecipeItem = RecipeItems[Index];

			if (const FItemInfoData* ItemData = CraftingContainerWidget->
				GetItemInfoData(RecipeItem->ResultItemName))
			{
				RecipeSelectButtonWidget->SetRecipeData(FRecipeSelectData(*RecipeItem,
				                                                          ItemData->Icon,
				                                                          ItemData->Name,
				                                                          ItemData->Description));
				RecipeSelectButtonWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			RecipeSelectButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	SetVisibility(ESlateVisibility::Visible);
}

void URecipeSelectWidget::CloseWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
