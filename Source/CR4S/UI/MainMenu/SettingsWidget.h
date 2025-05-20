#pragma once

#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"

class UButton;

UCLASS()
class CR4S_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnExitButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
