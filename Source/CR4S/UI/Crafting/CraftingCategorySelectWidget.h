#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CraftingCategorySelectWidget.generated.h"

class UCraftingCategoryButtonWidget;
class UCraftingContainerWidget;

UCLASS()
class CR4S_API UCraftingCategorySelectWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingContainerWidget* CraftingContainerWidget);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Initialize")
	TSubclassOf<UCraftingCategoryButtonWidget> CategoryButtonWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Initialize")
	TObjectPtr<UDataTable> RecipeCategoryDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "Initialize")
	FGameplayTag CookingRecipeTag;

	UPROPERTY()
	TArray<TObjectPtr<UCraftingCategoryButtonWidget>> CraftingCategoryWidgets;
	UPROPERTY()
	TArray<TObjectPtr<UCraftingCategoryButtonWidget>> CookingCategoryWidgets;

#pragma endregion

#pragma region Update

public:
	static void UpdateWidget(TArray<TObjectPtr<UCraftingCategoryButtonWidget>>& CategoryWidgets, bool IsCookingRecipe);
	void UpdateCategories(const int32 NewCraftingDifficulty);

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ButtonContainer;

#pragma endregion
};
