#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDummySlotWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UInventoryDummySlotWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	void SetDummy(UTexture2D* ItemIcon, const int32 ItemCount) const;
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountTextBlock;
	
#pragma endregion
};
