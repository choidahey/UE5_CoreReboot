#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecipeSelectWidget.generated.h"

struct FRecipeCategoryData;
class URecipeSelectButtonWidget;

UCLASS()
class CR4S_API URecipeSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Initialize

public:
	void InitWidget() const;
	
#pragma endregion 
	
#pragma region ToggleWidget

public:
	void OpenWidget(const FRecipeCategoryData& RecipeCategoryData);
	void CloseWidget();
	
#pragma endregion

#pragma region BindWidget;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CraftingCategoryIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CraftingCategoryName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ButtonContainer;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URecipeSelectButtonWidget> RecipeSelectButtonWidgetClass;
	UPROPERTY()
	TArray<TObjectPtr<URecipeSelectButtonWidget>> RecipeSelectButtonWidgets;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> ItemRecipeData;
	
#pragma endregion 
};
