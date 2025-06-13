#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "PlayerItemSlotWidget.generated.h"

class UQuickSlotInventoryComponent;
class UPlayerInventoryComponent;

UCLASS()
class CR4S_API UPlayerItemSlotWidget : public UBaseItemSlotWidget
{
	GENERATED_BODY()

#pragma region UBaseItemSlotWidget Override

public:
	UPlayerItemSlotWidget(const FObjectInitializer& ObjectInitializer);

	virtual void InitSlotWidgetData(UBaseInventoryWidget* NewInventoryWidget, UBaseInventoryItem* NewItem) override;

private:
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	
#pragma endregion
	
#pragma region Input

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void MoveItemToQuickSlot(const int32 Index) const;

#pragma endregion
};
