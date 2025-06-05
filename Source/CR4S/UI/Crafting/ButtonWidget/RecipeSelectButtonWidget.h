#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/ItemRecipeData.h"
#include "RecipeSelectButtonWidget.generated.h"

class UCraftingWidget;
class UCraftingContainerWidget;
struct FItemRecipeData;

USTRUCT()
struct FRecipeSelectData
{
	GENERATED_BODY()

	FItemRecipeData RecipeData = FItemRecipeData();
	UPROPERTY()
	TObjectPtr<UTexture2D> IconTexture = nullptr;
	FText RecipeName = FText();
};

UCLASS()
class CR4S_API URecipeSelectButtonWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingContainerWidget* NewCraftingContainerWidget);

private:
	UPROPERTY()
	TObjectPtr<UCraftingContainerWidget> CraftingContainerWidget;

	UPROPERTY()
	TObjectPtr<UCraftingWidget> CraftingWidget;
	
#pragma endregion

#pragma region OpenCraftingWidget

public:
	UFUNCTION()
	void OpenCraftingWidget();
	
#pragma endregion 

#pragma region RecipeData

public:
	void SetRecipeData(const FRecipeSelectData& RecipeData) const;

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RecipeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> RecipeIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RecipeName;

#pragma endregion
};
