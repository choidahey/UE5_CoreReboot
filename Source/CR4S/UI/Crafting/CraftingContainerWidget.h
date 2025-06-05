#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingContainerWidget.generated.h"

struct FRecipeCategoryData;
class UCraftingWidget;

UCLASS()
class CR4S_API UCraftingContainerWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget();
	void UpdateWidget(const int32 NewCraftingDifficulty);
	
#pragma endregion

#pragma region ToggleWidget

public:
	UFUNCTION()
	void OpenRecipeSelectWidget(const FRecipeCategoryData& RecipeCategoryData);
	void CloseRecipeSelectWidget();

	UFUNCTION()
	void OpenCraftingWidget();
	void CloseCraftingWidget();
	
#pragma endregion 

#pragma region BindWidget

public:
	FORCEINLINE UCraftingWidget* GetCraftingWidget() const { return CraftingWidget; }
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCraftingCategorySelectWidget> CraftingCategorySelectWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URecipeSelectWidget> RecipeSelectWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingWidget> CraftingWidget;

#pragma endregion

#pragma region RecipeData

private:
	int32 CraftingDifficulty = 0;
	
#pragma endregion 
};
