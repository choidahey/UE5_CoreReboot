#pragma once

#include "Blueprint/UserWidget.h"
#include "ProgressBarWidget.generated.h"

class UHorizontalBox;
class UTextBlock;
class UBorder;
class UProgressBar;
class USpacer;

UCLASS()
class CR4S_API UProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

public:
	void SetPercent(float Value);
	void SetFillColorAndOpacity(FColor Color);
	void SetCurrentValueText(const FText& Text);

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentValueText;
	UPROPERTY(meta = (BindWidget))
	USpacer* CurrentValueSpacer;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;
	UPROPERTY(meta = (BindWidget))
	UBorder* ProgressBarBorder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feature|CurrentValue")
	bool bShowCurrentValue = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feature|CurrentValue", meta = (EditCondition = "bShowCurrentValue"))
	int32 CurrentValueFontSize = 15;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feature|CurrentValue", meta = (EditCondition = "bShowCurrentValue"))
	float CurrentValuePadding = 27.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feature|ProgressBar")
	FMargin ProgressBarPadding = FMargin(45.f, 10.f, 25.f, 10.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feature|ProgressBar")
	FLinearColor FillColor = FLinearColor::White;

};
