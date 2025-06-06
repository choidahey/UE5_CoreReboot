#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/RecipeCategoryData.h"
#include "CraftingCategoryButtonWidget.generated.h"

class UCraftingContainerWidget;

UCLASS()
class CR4S_API UCraftingCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Override

public:
	UCraftingCategoryButtonWidget(const FObjectInitializer& ObjectInitializer);

#pragma endregion

#pragma region Initialize

public:
	void InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget,
	                const FRecipeCategoryData& NewRecipeCategoryData);

private:
	UPROPERTY()
	TObjectPtr<UCraftingContainerWidget> CraftingContainerWidget;

	FRecipeCategoryData RecipeCategoryData;

#pragma endregion

#pragma region ToggleRecipeSelectWidget

public:
	UFUNCTION()
	void OpenRecipeSelectWidget();

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CraftingCategoryButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScaleBox> ScaleBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CraftingCategoryIcon;

#pragma endregion
};
