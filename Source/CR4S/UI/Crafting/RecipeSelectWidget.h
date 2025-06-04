#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecipeSelectWidget.generated.h"

class URecipeSelectButtonWidget;

UCLASS()
class CR4S_API URecipeSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Initialize

public:
	void InitWidget();
	
#pragma endregion 
	
#pragma region ToggleWidget

public:
	void OpenWidget();
	void CloseWidget();
	
#pragma endregion

#pragma region BindWidget;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CraftingCategoryImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CraftingCategoryName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ButtonContainer;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URecipeSelectButtonWidget> RecipeSelectButtonWidgetClass;
	UPROPERTY()
	TArray<TObjectPtr<URecipeSelectButtonWidget>> RecipeSelectButtonWidgets;
	
#pragma endregion 
};
