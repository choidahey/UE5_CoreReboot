#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "BaseInventoryWidget.generated.h"

class UBaseInventoryComponent;
class UInventorySlotWidget;
class UButton;

UCLASS(Abstract, Blueprintable)
class CR4S_API UBaseInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	UFUNCTION(BlueprintCallable, Category = "InventoryWidget|Initalize")
	void InitInventoryWidget(UBaseInventoryComponent* InventoryComponent);
	
#pragma endregion
	
#pragma region BindWidget

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> InventoryBox;

#pragma endregion

#pragma region ItemWidget
	
protected:
	UFUNCTION()
	void UpdateItemSlotWidget(UBaseInventoryItem* Item);

	UPROPERTY()
	TArray<TObjectPtr<UInventorySlotWidget>> InventorySlotWidgets;
	
#pragma endregion
	
	
};
