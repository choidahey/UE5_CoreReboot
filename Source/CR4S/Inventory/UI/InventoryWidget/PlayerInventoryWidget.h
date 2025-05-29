#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "PlayerInventoryWidget.generated.h"

UCLASS()
class CR4S_API UPlayerInventoryWidget : public UBaseInventoryWidget
{
	GENERATED_BODY()

#pragma region Input

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
#pragma endregion 
};
