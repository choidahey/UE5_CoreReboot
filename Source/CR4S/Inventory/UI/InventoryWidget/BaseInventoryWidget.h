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
	UBaseInventoryWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable, Category = "InventoryWidget|Initalize")
	virtual void InitWidget(ASurvivalHUD* SurvivalHUD, bool bNewCanSort);

	virtual void ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent);

	virtual void UnBoundWidgetDelegate();

	FORCEINLINE UBaseInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

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

#pragma region ItemSlotWidget

protected:
	UFUNCTION()
	virtual void UpdateItemSlotWidget(int32 SlotIndex, UBaseInventoryItem* Item);

	UPROPERTY()
	TArray<TObjectPtr<UBaseItemSlotWidget>> ItemSlotWidgets;

#pragma endregion

#pragma region Input

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FORCEINLINE bool CanSort() const { return bCanSort; }
	FORCEINLINE bool CanDrag() const { return bCanDrag; }
	FORCEINLINE bool CanDrop() const { return bCanDrop; }
	FORCEINLINE bool CanRemoveItem() const { return bCanRemoveItem; }
	FORCEINLINE bool CanMoveItem() const { return bCanMoveItem; }

	FORCEINLINE void SetCanSort(const bool bNewCanSort) { bCanSort = bNewCanSort; }
	FORCEINLINE void SetCanDrag(const bool bNewCanDrag) { bCanDrag = bNewCanDrag; }
	FORCEINLINE void SetCanDrop(const bool bNewCanDrop) { bCanDrop = bNewCanDrop; }
	FORCEINLINE void SetCanRemoveItem(const bool bNewCanRemoveItem) { bCanRemoveItem = bNewCanRemoveItem; }
	FORCEINLINE void SetCanMoveItem(const bool bNewCanMoveItem) { bCanMoveItem = bNewCanMoveItem; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bCanSort;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bCanDrag;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bCanDrop;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bCanRemoveItem;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	bool bCanMoveItem;

#pragma endregion
};
