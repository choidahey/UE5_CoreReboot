#pragma once

#include "CoreMinimal.h"
#include "IngredientsWidget.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/ItemData.h"
#include "Gimmick/Data/ItemRecipeData.h"
#include "Inventory/Components/PlayerInventoryComponent.h"

#include "CraftingWidget.generated.h"

struct FRecipeSelectData;
class UButton;
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

	virtual void NativeConstruct() override;
	
#pragma endregion 
	
#pragma region Initialize

public:
	void InitWidget(UPlayerInventoryComponent* NewPlayerInventoryComponent);
	void UpdateResultItem() const;

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

public:
	void CreateResultItem(const FGameplayTagContainer& ItemTags);
	
private:
	UFUNCTION()
	void CraftItem();

	void PlayCraftingSound() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crafitng|Sound")
	TObjectPtr<USoundBase> CraftingSound;
	
	bool bCanCraft;
	
	FItemRecipeData ItemRecipeData;

	TArray<FIngredientData> CurrentIngredients;

	UPROPERTY()
	TObjectPtr<UBaseInventoryItem> ResultItem;
	
#pragma endregion

#pragma region Tooltip

private:
	UFUNCTION()
	UWidget* ShowToolTip();

	UPROPERTY(EditDefaultsOnly, Category = "ItemTooltip")
	TSubclassOf<UItemTooltipWidget> ItemTooltipWidgetClass;

	UPROPERTY()
	TObjectPtr<UItemTooltipWidget> ItemTooltipWidget;
	
#pragma endregion 

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftingComplete);
	UPROPERTY(BlueprintAssignable, Category = "Crafting")
	FOnCraftingComplete OnCraftingComplete;
	
#pragma endregion 
};
