#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UDummyItemSlotWidget;
class UPlayerInventoryWidget;
class UBaseInventoryComponent;
class UBorder;
class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UInventorySlotWidget : public UBaseItemSlotWidget
{
	GENERATED_BODY()
	
#pragma region Drag And Drop
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "DragAndDrop")
	TSubclassOf<UDummyItemSlotWidget> DummySlotWidgetClass;
	
#pragma endregion
	
};
