#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gimmick/Data/ItemData.h"
#include "ItemTooltipWidget.generated.h"

class UBaseInventoryItem;
class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	
#pragma endregion
	
#pragma region Initialize
	
public:
	void InitWidget(UBaseInventoryItem* NewItem);
	
	UPROPERTY()
	TObjectPtr<UBaseInventoryItem> CurrentItem;
	
#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescription;
	
	
#pragma endregion 
	
};
