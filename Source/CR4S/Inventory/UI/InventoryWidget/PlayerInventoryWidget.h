#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "PlayerInventoryWidget.generated.h"

UCLASS()
class CR4S_API UPlayerInventoryWidget : public UBaseInventoryWidget
{
	GENERATED_BODY()

#pragma region UBaseInventoryWidget Override

public:
	virtual void InitWidget(ASurvivalHUD* SurvivalHUD,
	                        UInventoryContainerWidget* InventoryContainerWidget) override;

	virtual void ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent, bool bCanDrag, bool bCanDrop) override;
	
#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SortButton;

#pragma endregion
};
