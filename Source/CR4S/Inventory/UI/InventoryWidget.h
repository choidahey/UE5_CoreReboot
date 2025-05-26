#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "InventoryWidget.generated.h"

class UVerticalBox;
class UButton;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class CR4S_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	UFUNCTION(BlueprintCallable, Category = "InventoryWidget|Initalize")
	void InitInventoryWidget(UInventoryComponent* InventorySystemComponent);
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> InventoryBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SortButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
#pragma endregion

#pragma region ItemWidget
	
private:
	UFUNCTION()
	void SetItemWidget(UBaseInventoryItem* Item);

	UPROPERTY()
	TArray<TObjectPtr<UInventorySlotWidget>> InventorySlotWidgets;
	
#pragma endregion
};
