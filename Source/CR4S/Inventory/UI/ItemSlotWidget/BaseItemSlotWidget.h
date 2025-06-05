#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseItemSlotWidget.generated.h"

class UBaseInventoryWidget;
class UInventoryContainerWidget;
class UDummyItemSlotWidget;
class UBorder;
class UTextBlock;
class UImage;
class UBaseInventoryComponent;
class UBaseInventoryItem;

UCLASS()
class CR4S_API UBaseItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Override

public:
	virtual void NativeConstruct() override;

#pragma endregion

#pragma region Initalize

public:
	UBaseItemSlotWidget(const FObjectInitializer& ObjectInitializer);
	void InitSlotWidget(int32 NewSlotIndex);
	void InitSlotWidgetData(const UBaseInventoryWidget* NewInventoryWidget, UBaseInventoryItem* NewItem);

	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE UBaseInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE void SetInventoryComponent(UBaseInventoryComponent* NewInventoryComponent)
	{
		InventoryComponent = NewInventoryComponent;
	}

protected:
	int32 SlotIndex;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY()
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;
	UPROPERTY()
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidget;

	bool bIsPlayerItemSlot;

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HoverImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountTextBlock;

#pragma endregion

#pragma region Item

public:
	void SetItem(UBaseInventoryItem* InItem);

	FORCEINLINE UBaseInventoryItem* GetCurrentItem() const { return CurrentItem; }

protected:
	UPROPERTY()
	TObjectPtr<UBaseInventoryItem> CurrentItem;

#pragma endregion

#pragma region Hover

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

#pragma endregion

#pragma region Drag And Drop

public:
	bool IsItemAllowedByFilter(const UBaseInventoryItem* Item) const;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	                                  UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;

	UPROPERTY(VisibleAnywhere, Category = "DragAndDrop")
	bool bCanDrag;
	UPROPERTY(VisibleAnywhere, Category = "DragAndDrop")
	bool bCanDrop;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDummyItemSlotWidget> DummySlotWidgetClass;

#pragma endregion

#pragma region Input

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Input")
	bool bCanRemoveItem;
	UPROPERTY(VisibleAnywhere, Category = "Input")
	bool bCanMoveItem;

#pragma endregion
};
