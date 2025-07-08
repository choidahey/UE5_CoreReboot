#include "UI/Common/ProgressBarWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Spacer.h"
#include "Components/Border.h"
#include "Blueprint/UserWidget.h"


void UProgressBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UProgressBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (CurrentValueText)
	{
		CurrentValueText->SetVisibility(bShowCurrentValue ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		if (bShowCurrentValue)
		{
			FSlateFontInfo FontInfo = CurrentValueText->GetFont();
			FontInfo.Size = CurrentValueFontSize;
			CurrentValueText->SetFont(FontInfo);
		}
	}

	if (CurrentValueSpacer)
	{
		CurrentValueSpacer->SetSize(FVector2D(CurrentValuePadding, 1.0f));
	}

	if (ProgressBarBorder)
	{
		ProgressBarBorder->SetPadding(ProgressBarPadding);
	}

	if (ProgressBar)
	{
		ProgressBar->SetFillColorAndOpacity(FillColor);
	}
}


void UProgressBarWidget::SetPercent(float Value)
{
	ProgressBar->SetPercent(Value);
}

void UProgressBarWidget::SetFillColorAndOpacity(FColor Color)
{
	ProgressBar->SetFillColorAndOpacity(Color);
}

void UProgressBarWidget::SetCurrentValueText(const FText& Text)
{
	CurrentValueText->SetText(Text);
}
