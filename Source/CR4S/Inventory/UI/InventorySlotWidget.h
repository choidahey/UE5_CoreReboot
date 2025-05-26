#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryComponent.h"
#include "InventorySlotWidget.generated.h"

class UInventoryDummySlotWidget;
class UInventoryWidget;
class UInventoryComponent;
class UBorder;
class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Override

public:
	virtual bool Initialize() override;
	
#pragma endregion

#pragma region Initalize

public:
	void InitWidget(UInventoryComponent* InInventorySystemComponent, UBaseInventoryItem* NewItem);

private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventorySystemComponent;
	
#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> CountTextBorder;
	
#pragma endregion
	
#pragma region Drag And Drop
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "DragAndDrop")
	TSubclassOf<UInventoryDummySlotWidget> DummySlotWidgetClass;
	
#pragma endregion
	
#pragma region Item

public:
	void SetItem(UBaseInventoryItem* InItem);

private:
	UPROPERTY()
	TObjectPtr<UBaseInventoryItem> CurrentItem;
	
#pragma endregion
};
