#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "UI/Crafting/ButtonWidget/RecipeSelectButtonWidget.h"
#include "HelperBotWorkshopWidget.generated.h"

class UHelperBotSlotWidget;
class UButton;
class UTextBlock;
class UHelperBotManagementWidget;
class UHelperBotRepairWidget;
class UCraftingWidget;

UCLASS()
class CR4S_API UHelperBotWorkshopWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initailize

public:
	void InitWidget(UPlayerInventoryComponent* PlayerInventoryComponent);
	void InitRecipeData();
	
#pragma endregion
	
#pragma region ToggleWidget

public:
	UFUNCTION()
	void CloseOtherWidget();

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> OpenOtherWidget;
	
#pragma endregion 

#pragma region Crafting

public:
	void CloseCraftingWidget() const;
	
private:
	UFUNCTION()
	void OpenCraftingWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
	FDataTableRowHandle HelperBotRecipeDataHandle;
	
	FRecipeSelectData HelperBotRecipeData;

#pragma endregion 

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnergyTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> SlotContainer;
	UPROPERTY()
	TArray<TObjectPtr<UHelperBotSlotWidget>> HelperBotSlots;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CraftingButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingWidget> CraftingWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHelperBotRepairWidget> HelperBotRepairWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHelperBotManagementWidget> HelperBotManagementWidget;
	
#pragma endregion 
	
};
