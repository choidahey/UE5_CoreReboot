#include "CraftingContainerWidget.h"

#include "CR4S.h"
#include "CraftingCategorySelectWidget.h"
#include "CraftingWidget.h"
#include "RecipeSelectWidget.h"
#include "Gimmick/Data/ItemRecipeData.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

void UCraftingContainerWidget::InitWidget(UPlayerInventoryComponent* NewPlayerInventoryComponent)
{
	PlayerInventoryComponent = NewPlayerInventoryComponent;

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

	if (IsValid(CraftingWidget))
	{
		CraftingWidget->InitWidget(this);
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

void UCraftingContainerWidget::OpenRecipeSelectWidget(const FRecipeCategoryData& RecipeCategoryData) const
{
	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->OpenWidget(RecipeCategoryData);
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

const FItemInfoData* UCraftingContainerWidget::GetItemInfoData(const FName RowName) const
{
	if (IsValid(ItemGimmickSubsystem))
	{
		return ItemGimmickSubsystem->FindItemInfoData(RowName);
	}

	return nullptr;
}

int32 UCraftingContainerWidget::GetItemCountByRowName(const FName RowName) const
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(PlayerInventoryComponent)))
	{
		return 0;
	}

	return PlayerInventoryComponent->GetItemCountByRowName(RowName);
}

void UCraftingContainerWidget::CraftItem(const FItemRecipeData& ItemRecipeData) const
{
	if (CR4S_VALIDATE(LogCraftingUI, IsValid(PlayerInventoryComponent)))
	{
		for (const auto& [IngredientItemName, Count] : ItemRecipeData.Ingredients)
		{
			PlayerInventoryComponent->RemoveItemByRowName(IngredientItemName, Count);
		}
		
		PlayerInventoryComponent->AddItem(ItemRecipeData.ResultItemName, ItemRecipeData.ResultCount);
	}
}

