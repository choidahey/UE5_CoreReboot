#include "UI/Common/BaseWindowWidget.h"
#include "UI/Common/ButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UBaseWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ApplyButton)
	{
		ApplyButton->OnClicked().AddUniqueDynamic(this, &UBaseWindowWidget::OnApplyButtonClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked().AddUniqueDynamic(this, &UBaseWindowWidget::OnBackButtonClicked);
	}
}

void UBaseWindowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (WindowTitle)
	{
		WindowTitle->SetText(TitleText);
	}

	if (WindowIcon && IconTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(IconTexture);
		Brush.ImageSize = FVector2D(80.f, 80.f);

		WindowIcon->SetBrush(Brush);
	}

	if (!bShowRightButton)
	{
		ApplyButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		ApplyButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (!bShowLeftButton)
	{
		BackButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		BackButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UBaseWindowWidget::OnApplyButtonClicked()
{
	OnApplyClicked.Broadcast();
}

void UBaseWindowWidget::OnBackButtonClicked()
{
	OnBackClicked.Broadcast();
}