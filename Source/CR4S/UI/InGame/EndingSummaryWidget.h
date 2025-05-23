#pragma once

#include "Blueprint/UserWidget.h"
#include "EndingSummaryWidget.generated.h"

class UTextBlock;
class UScrollBox;

DECLARE_DELEGATE(FOnEndingSequenceFinished);

UCLASS()
class CR4S_API UEndingSummaryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void PlayEndingSequence();

	void SetEndingSummaryText();

protected:
	void OnEndingSequenceFinished();
	void ScrollEndingSummary();

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* EndingSummaryText;
	UPROPERTY(Meta = (BindWidget))
	UScrollBox* EndingSummaryScrollBox;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EndingText")
	float ScrollSpeed = 20.0f;
	float CurrentOffset = 0.0f;

	FTimerHandle ScrollTimerHandle;



	virtual void NativeConstruct() override;
};
