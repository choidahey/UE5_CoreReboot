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

	FRecipeSelectData()
	{
	}

	FRecipeSelectData(const FItemRecipeData& NewRecipeData,
	                  UTexture2D* NewIcon,
	                  const FText& NewName,
	                  const FText& NewDescription)
		: RecipeData(NewRecipeData),
		  Icon(NewIcon),
		  Name(NewName),
		  Description(NewDescription)
	{
	}

	FItemRecipeData RecipeData = FItemRecipeData();
	UPROPERTY()
	TObjectPtr<UTexture2D> Icon = nullptr;
	FText Name = FText();
	FText Description = FText();
};

UCLASS()
class CR4S_API URecipeSelectButtonWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingWidget* NewCraftingWidget);

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
	void SetRecipeData(const FRecipeSelectData& NewRecipeData);

private:
	FRecipeSelectData RecipeSelectData = FRecipeSelectData();

	FText ItemDescription = FText();

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
