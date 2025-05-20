#pragma once

#include "Blueprint/UserWidget.h"
#include "ConfirmWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class CR4S_API UConfirmWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnYesButtonClicked();
	UFUNCTION()
	void OnNoButtonClicked();

	void SetMessageText(FText NewMessage);

	UPROPERTY(meta = (BindWidget))
	UButton* YesButton;
	UPROPERTY(meta = (BindWidget))
	UButton* NoButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;
};
