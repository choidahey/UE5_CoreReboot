#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "UI/InGame/SurvivalHUD.h"
#include "BaseInventoryWidget.generated.h"

class UBaseItemSlotWidget;
class UInventoryContainerWidget;
class UBaseInventoryComponent;
class UInventorySlotWidget;
class UButton;

UCLASS(Abstract, Blueprintable)
class CR4S_API UBaseInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	UFUNCTION(BlueprintCallable, Category = "InventoryWidget|Initalize")
	virtual void InitWidget(ASurvivalHUD* SurvivalHUD);

	virtual void ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent, bool bCanDrag, bool bCanDrop);

	void UnBoundOnItemSlotChanged();
	
protected:
	UPROPERTY()
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidget;

#pragma endregion

#pragma region BindWidget

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> ItemSlotWidgetContainer;

#pragma endregion

#pragma region ItemWidget

protected:
	UFUNCTION()
	void UpdateItemSlotWidget(UBaseInventoryItem* Item);
	
	UPROPERTY()
	TArray<TObjectPtr<UBaseItemSlotWidget>> ItemSlotWidgets;

#pragma endregion
};
