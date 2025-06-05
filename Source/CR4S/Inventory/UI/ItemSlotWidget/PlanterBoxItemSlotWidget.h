#pragma once

#include "CoreMinimal.h"
#include "BaseItemSlotWidget.h"
#include "PlanterBoxItemSlotWidget.generated.h"

UCLASS()
class CR4S_API UPlanterBoxItemSlotWidget : public UBaseItemSlotWidget
{
	GENERATED_BODY()

#pragma region UBaseItemSlotWidget Override

public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
#pragma endregion 
};
