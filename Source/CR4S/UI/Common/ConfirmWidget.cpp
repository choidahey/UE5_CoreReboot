#include "UI/Common/ConfirmWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	if (YesButton)
	{
		YesButton->OnClicked.AddUniqueDynamic(this, &UConfirmWidget::OnYesButtonClicked);
	}
	if (NoButton)
	{
		NoButton->OnClicked.AddUniqueDynamic(this, &UConfirmWidget::OnNoButtonClicked);
	}
}

void UConfirmWidget::OnYesButtonClicked()
{
	if (OnYes.IsBound())
	{
		OnYes.Execute();
	}
}

void UConfirmWidget::OnNoButtonClicked()
{
	if (OnNo.IsBound())
	{
		OnNo.Execute();
	}
}

void UConfirmWidget::Confirm(const FText& Message)
{
	MessageText->SetText(Message);
	SetVisibility(ESlateVisibility::Visible);
}

void UConfirmWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (YesButton)
	{
		YesButton->OnClicked.RemoveAll(this);
	}
	if (NoButton)
	{
		NoButton->OnClicked.RemoveAll(this);
	}
}