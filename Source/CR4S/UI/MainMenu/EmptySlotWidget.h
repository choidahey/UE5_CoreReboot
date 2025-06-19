#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "EmptySlotWidget.generated.h"

class UButton;

UCLASS()
class CR4S_API UEmptySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnButtonClickedEvent& OnClicked() { return EmptySlotButton->OnClicked; }


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EmptySlotButton;
};
