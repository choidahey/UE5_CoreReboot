#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/ItemRecipeData.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "RecipeSelectWidget.generated.h"

class UCraftingWidget;
struct FItemInfoData;
class UCraftingContainerWidget;
struct FRecipeCategoryData;
class URecipeSelectButtonWidget;

UCLASS()
class CR4S_API URecipeSelectWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UCraftingWidget* NewCraftingWidget);

private:
	UPROPERTY()
	TObjectPtr<UCraftingWidget> CratingWidget;

#pragma endregion

#pragma region ToggleWidget

public:
	void OpenWidget(UTexture2D* Icon, const FText& Name, TArray<const FItemRecipeData*> RecipeItems);
	void CloseWidget();
	
#pragma endregion

#pragma region BindWidget;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CraftingCategoryIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CraftingCategoryName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ButtonContainer;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<URecipeSelectButtonWidget> RecipeSelectButtonWidgetClass;
	UPROPERTY()
	TArray<TObjectPtr<URecipeSelectButtonWidget>> RecipeSelectButtonWidgets;

#pragma endregion
};
