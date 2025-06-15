#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class UTextBlock;
class UImage;
struct FItemInfoData;

UCLASS()
class CR4S_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize
	
public:
	void InitWidget(const FItemInfoData& ItemInfo) const;

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
