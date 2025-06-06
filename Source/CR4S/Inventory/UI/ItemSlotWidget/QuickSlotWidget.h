#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "QuickSlotWidget.generated.h"

UCLASS()
class CR4S_API UQuickSlotWidget : public UBaseItemSlotWidget
{
	GENERATED_BODY()

#pragma region Drag And Drop

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
							  UDragDropOperation* InOperation) override;

#pragma endregion

#pragma region Input

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

#pragma endregion 
};
