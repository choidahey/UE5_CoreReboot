#include "CraftingCategorySelectWidget.h"

#include "ButtonWidget/CraftingCategoryButtonWidget.h"
#include "Components/PanelWidget.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Gimmick/Data/RecipeCategoryData.h"

void UCraftingCategorySelectWidget::InitWidget(UCraftingContainerWidget* CraftingContainerWidget) const
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(RecipeCategoryDataTable)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ButtonContainer)))
	{
		return;
	}

	TArray<FName> RowNames = RecipeCategoryDataTable->GetRowNames();

	int32 Index = 0;
	for (UWidget* Widget : ButtonContainer->GetAllChildren())
	{
		UCraftingCategoryButtonWidget* CraftingCategoryButtonWidget = Cast<UCraftingCategoryButtonWidget>(Widget);
		if (IsValid(CraftingCategoryButtonWidget) && RowNames.IsValidIndex(Index))
		{
			const FRecipeCategoryData* RecipeCategoryData
				= RecipeCategoryDataTable->FindRow<FRecipeCategoryData>(RowNames[Index],
				                                                         TEXT("RecipeCategoryData"));

			CraftingCategoryButtonWidget->InitWidget(CraftingContainerWidget, *RecipeCategoryData);

			Index++;
		}
	}
}
