#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class CR4S_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initalize

public:
	void SetInteractionText(const FText& InInteractionText) const;
	
#pragma endregion
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> InteractionKeyIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractionTextBlock;
	
#pragma endregion
};
