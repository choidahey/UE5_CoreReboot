#pragma once

#include "Blueprint/UserWidget.h"
#include "ConfirmWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_DELEGATE(FOnYes);
DECLARE_DYNAMIC_DELEGATE(FOnNo);

UCLASS()
class CR4S_API UConfirmWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Confirm(const FText& Message);

	FOnYes OnYes;
	FOnNo OnNo;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnYesButtonClicked();
	UFUNCTION()
	void OnNoButtonClicked();



	UPROPERTY(meta = (BindWidget))
	UButton* YesButton;
	UPROPERTY(meta = (BindWidget))
	UButton* NoButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;


};
