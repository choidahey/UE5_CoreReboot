#include "CraftingWidget.h"

#include "CR4S.h"
#include "CraftingContainerWidget.h"
#include "ButtonWidget/RecipeSelectButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/ItemData.h"

void UCraftingWidget::InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget)
{
	CraftingContainerWidget = NewCraftingContainerWidget;

	if (IsValid(IngredientsContainer))
	{
		for (UWidget* Widget : IngredientsContainer->GetAllChildren())
		{
			UIngredientWidget* IngredientWidget = Cast<UIngredientWidget>(Widget);
			if (IsValid(IngredientWidget))
			{
				IngredientWidgets.AddUnique(IngredientWidget);
				IngredientWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (IsValid(CraftButton))
	{
		CraftButton->OnClicked.AddUniqueDynamic(this, &ThisClass::CraftItem);
		CraftButton->SetIsEnabled(false);
	}
}

void UCraftingWidget::OpenWidget(const FRecipeSelectData& RecipeSelectData)
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemIcon)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemName)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemDescription)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftButton)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingContainerWidget)))
	{
		return;
	}

	ItemRecipeData = RecipeSelectData.RecipeData;

	ItemIcon->SetBrushFromTexture(RecipeSelectData.Icon, true);
	ItemName->SetText(RecipeSelectData.Name);
	ItemDescription->SetText(RecipeSelectData.Description);

	bCanCraft = true;
	CurrentIngredients.Empty();

	const TArray<FRecipeIngredient>& Ingredients = RecipeSelectData.RecipeData.Ingredients;
	for (int32 Index = 0; Index < IngredientWidgets.Num(); Index++)
	{
		UIngredientWidget* IngredientWidget = IngredientWidgets[Index];
		if (!IsValid(IngredientWidget))
		{
			continue;
		}

		if (Ingredients.IsValidIndex(Index))
		{
			const FRecipeIngredient& Ingredient = Ingredients[Index];
			if (const FItemInfoData* ItemData = CraftingContainerWidget->GetItemInfoData(Ingredient.IngredientItemName))
			{
				const int32 CurrentCount = CraftingContainerWidget->
					GetItemCountByRowName(Ingredient.IngredientItemName);
				const int32 RequiredCount = Ingredient.Count;

				if (bCanCraft && CurrentCount < RequiredCount)
				{
					bCanCraft = false;
				}

				FIngredientData IngredientData = FIngredientData(ItemData->Icon,
				                                                 ItemData->Name,
				                                                 CurrentCount,
				                                                 RequiredCount);
				CurrentIngredients.Add(IngredientData);

				IngredientWidget->InitWidget(IngredientData);
				IngredientWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			IngredientWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	CraftButton->SetIsEnabled(bCanCraft);

	SetVisibility(ESlateVisibility::Visible);
}

void UCraftingWidget::CloseWidget()
{
	for (UIngredientWidget* IngredientWidget : IngredientWidgets)
	{
		IngredientWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UCraftingWidget::CraftItem()
{
	if (CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingContainerWidget)))
	{
		CraftingContainerWidget->CraftItem(ItemRecipeData);

		for (int32 Index = 0; Index < CurrentIngredients.Num(); Index++)
		{
			if (!IngredientWidgets.IsValidIndex(Index))
			{
				return;
			}
			
			UIngredientWidget* IngredientWidget = IngredientWidgets[Index];
			if (IsValid(IngredientWidget))
			{
				FIngredientData& IngredientData = CurrentIngredients[Index];
				
				IngredientData.CurrentCount -= IngredientData.RequiredCount;
				
				IngredientWidget->InitWidget(IngredientData);
				
				if (bCanCraft && IngredientData.CurrentCount < IngredientData.RequiredCount)
				{
					bCanCraft = false;
				}
			}

			CraftButton->SetIsEnabled(bCanCraft);
		}
	}
}
