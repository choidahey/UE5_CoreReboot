#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UButton;
class UInventorySystemComponent;
class UInventorySlotWidget;
class UWrapBox;

UCLASS()
class CR4S_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	UFUNCTION(BlueprintCallable, Category = "InventoryWidget|Initalize")
	void InitInventoryWidget(UInventorySystemComponent* InventorySystemComponent, const int32 InventorySize);
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> InventoryWrapBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SortButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
#pragma endregion

#pragma region ItemWidget
	
private:
	UFUNCTION()
	void SetItemWidget(const int32 SlotIndex, const FName& RowName, UTexture2D* ItemIcon, const int32 ItemCount);
	
	UPROPERTY(EditDefaultsOnly, Category = "ItemWidget")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UInventorySlotWidget>> InventorySlotWidgets;
	
#pragma endregion
};
