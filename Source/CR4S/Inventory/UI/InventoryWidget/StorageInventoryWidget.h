#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "StorageInventoryWidget.generated.h"

UCLASS()
class CR4S_API UStorageInventoryWidget : public UBaseInventoryWidget
{
	GENERATED_BODY()

#pragma region UBaseInventoryWidget Override

public:
	virtual void InitWidget(ASurvivalHUD* SurvivalHUD) override;
	
#pragma endregion

#pragma region StorageInventoryWidget

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBaseItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxSlotCount = 100;
	
#pragma endregion 
};
