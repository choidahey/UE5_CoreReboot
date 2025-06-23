#pragma once

#include "CoreMinimal.h"
#include "IngredientsWidget.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/ItemRecipeData.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

#include "CraftingWidget.generated.h"

struct FRecipeIngredient;
class UButton;
struct FRecipeSelectData;
class UIngredientsWidget;
class UCraftingContainerWidget;
class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UCraftingWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Override

public:
	UCraftingWidget(const FObjectInitializer& ObjectInitializer);
	
#pragma endregion 
	
#pragma region Initialize

public:
	void InitWidget(UPlayerInventoryComponent* NewPlayerInventoryComponent);

private:
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;

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
	TArray<TObjectPtr<UIngredientsWidget>> IngredientWidgets;

#pragma endregion

#pragma region Crafting

private:
	UFUNCTION()
	void CraftItem();
	
	bool bCanCraft;
	
	FItemRecipeData ItemRecipeData;

	TArray<FIngredientData> CurrentIngredients;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftingComplete);
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnCraftingComplete OnCraftingComplete;
	
#pragma endregion 
};
