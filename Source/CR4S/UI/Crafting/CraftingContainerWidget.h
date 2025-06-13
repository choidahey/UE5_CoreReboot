#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingContainerWidget.generated.h"

struct FItemRecipeData;
class UPlayerInventoryComponent;
struct FRecipeSelectData;
class UItemGimmickSubsystem;
struct FItemInfoData;
struct FRecipeCategoryData;
class UCraftingWidget;

UCLASS()
class CR4S_API UCraftingContainerWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(UPlayerInventoryComponent* PlayerInventoryComponent);
	void UpdateWidget(const int32 NewCraftingDifficulty);

private:
	int32 CraftingDifficulty = 0;
	
#pragma endregion

#pragma region ToggleWidget

public:
	UFUNCTION()
	void OpenRecipeSelectWidget(const FRecipeCategoryData& RecipeCategoryData) const;
	void CloseRecipeSelectWidget() const;
	
	void CloseCraftingWidget() const;
	
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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> ItemRecipeData;

#pragma endregion
	
};
