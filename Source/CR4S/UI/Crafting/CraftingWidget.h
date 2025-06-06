#pragma once

#include "CoreMinimal.h"
#include "IngredientWidget.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/ItemRecipeData.h"

#include "CraftingWidget.generated.h"

struct FRecipeIngredient;
class UButton;
struct FRecipeSelectData;
class UIngredientWidget;
class UCraftingContainerWidget;
class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UCraftingWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget);

private:
	UPROPERTY()
	TObjectPtr<UCraftingContainerWidget> CraftingContainerWidget;

#pragma endregion

#pragma region ToggleWidget

public:
	void OpenWidget(const FRecipeSelectData& RecipeSelectData);
	void CloseWidget();

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescription;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CraftButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> IngredientsContainer;

	UPROPERTY()
	TArray<TObjectPtr<UIngredientWidget>> IngredientWidgets;

#pragma endregion

#pragma region Crafting
	
private:
	UFUNCTION()
	void CraftItem();
	
	bool bCanCraft = false;

	FItemRecipeData ItemRecipeData = FItemRecipeData();

	TArray<FIngredientData> CurrentIngredients;
	
#pragma endregion
};
