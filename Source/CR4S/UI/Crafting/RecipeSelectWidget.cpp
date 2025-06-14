#include "RecipeSelectWidget.h"

#include "CR4S.h"
#include "CraftingContainerWidget.h"
#include "ButtonWidget/RecipeSelectButtonWidget.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/ItemData.h"

void URecipeSelectWidget::InitWidget(UCraftingWidget* NewCraftingWidget)
{
	if (IsValid(ButtonContainer))
	{
		for (UWidget* Widget : ButtonContainer->GetAllChildren())
		{
			URecipeSelectButtonWidget* RecipeSelectButtonWidget = Cast<URecipeSelectButtonWidget>(Widget);
			if (IsValid(RecipeSelectButtonWidget))
			{
				RecipeSelectButtonWidgets.AddUnique(RecipeSelectButtonWidget);
				RecipeSelectButtonWidget->InitWidget(NewCraftingWidget);
				RecipeSelectButtonWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void URecipeSelectWidget::OpenWidget(UTexture2D* Icon,
                                     const FText& Name,
                                     TArray<const FItemRecipeData*> RecipeItems)
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(ScrollBox)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingCategoryIcon)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftingCategoryName)))
	{
		return;
	}

	if (RecipeItems.Num() == 0)
	{
		CloseWidget();
	}
	
	ScrollBox->ScrollToStart();

	CraftingCategoryIcon->SetBrushFromTexture(Icon, true);
	CraftingCategoryName->SetText(Name);

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
			const UDataTable* DataTable = RecipeItem->ItemInfoDataHandle.DataTable;
			if (!CR4S_VALIDATE(LogCraftingUI, IsValid(DataTable)))
			{
				continue;
			}

			const FName RowName = RecipeItem->ItemInfoDataHandle.RowName;
			if (const FItemInfoData* ItemData = DataTable->FindRow<FItemInfoData>(RowName, TEXT("Item")))
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
