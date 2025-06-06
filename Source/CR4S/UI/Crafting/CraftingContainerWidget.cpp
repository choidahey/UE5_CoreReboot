#include "CraftingContainerWidget.h"

#include "CR4S.h"
#include "CraftingCategorySelectWidget.h"
#include "CraftingWidget.h"
#include "RecipeSelectWidget.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"

void UCraftingContainerWidget::InitWidget()
{
	if (IsValid(CraftingCategorySelectWidget))
	{
		CraftingCategorySelectWidget->InitWidget(this);
	}

	CloseCraftingWidget();
	
	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->InitWidget(this);

		CloseRecipeSelectWidget();
	}

	ItemGimmickSubsystem = GetWorld()->GetSubsystem<UItemGimmickSubsystem>();
}

void UCraftingContainerWidget::UpdateWidget(const int32 NewCraftingDifficulty)
{
	CraftingDifficulty = NewCraftingDifficulty;
	
	CR4S_Log(LogCraftingUI, Warning, TEXT("Crafting Difficulty: %d"), CraftingDifficulty);
	
	CloseCraftingWidget();
	
	CloseRecipeSelectWidget();
}

void UCraftingContainerWidget::OpenRecipeSelectWidget(const FRecipeCategoryData& RecipeCategoryData)
{
	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->OpenWidget(RecipeCategoryData);
	}

	CloseCraftingWidget();
}

void UCraftingContainerWidget::CloseRecipeSelectWidget()
{
	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->CloseWidget();
	}
}

void UCraftingContainerWidget::OpenCraftingWidget()
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->OpenWidget();
	}
}

void UCraftingContainerWidget::CloseCraftingWidget()
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->CloseWidget();
	}
}

const FItemInfoData* UCraftingContainerWidget::GetItemInfoData(const FName RowName) const
{
	if (IsValid(ItemGimmickSubsystem))
	{
		return ItemGimmickSubsystem->FindItemInfoData(RowName);
	}

	return nullptr;
}
