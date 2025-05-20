#pragma once

#include "Blueprint/UserWidget.h"
#include "CreditsWidget.generated.h"

class UButton;

UCLASS()
class CR4S_API UCreditsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnExitButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
