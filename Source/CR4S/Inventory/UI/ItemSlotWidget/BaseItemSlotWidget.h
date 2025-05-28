#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseItemSlotWidget.generated.h"

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
	
#pragma region Initalize

public:
	void InitWidget(UBaseInventoryItem* NewItem, bool bNewCanDrag, bool bNewCanDrop);
	
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

#pragma region Item

public:
	void SetItem(UBaseInventoryItem* InItem);
	void EmptyItem();

protected:
	UPROPERTY()
	TObjectPtr<UBaseInventoryItem> CurrentItem;
	
#pragma endregion
	
#pragma region Drag And Drop
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UPROPERTY(EditDefaultsOnly)
	bool bCanDrag;
	UPROPERTY(EditDefaultsOnly)
	bool bCanDrop;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDummyItemSlotWidget> DummySlotWidgetClass;
	
#pragma endregion
};
