#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelperBotRepairWidget.generated.h"

class UHelperBotManagementWidget;
class UIngredientsWidget;
class UButton;

UCLASS()
class CR4S_API UHelperBotRepairWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initilize

public:
	void InitWidget(UHelperBotManagementWidget* NewHelperBotManagementWidget);
	void UpdateIngredients();

private:
	UPROPERTY()
	TObjectPtr<UHelperBotManagementWidget> HelperBotManagementWidget;
	
#pragma endregion

#pragma region ToggleWidget

public:
	UFUNCTION()
	void OpenWidget();
	UFUNCTION()
	void CloseWidget();
	
#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> IngredientsContainer;
	UPROPERTY()
	TArray<TObjectPtr<UIngredientsWidget>> IngredientsWidgets;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RepairButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;

#pragma endregion 
};
