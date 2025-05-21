#include "UI/Common/BaseWindowWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBaseWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ApplyButton)
	{
		ApplyButton->OnClicked.AddUniqueDynamic(this, &UBaseWindowWidget::OnApplyButtonClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddUniqueDynamic(this, &UBaseWindowWidget::OnBackButtonClicked);
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

void UBaseWindowWidget::SetTitleText(FText NewText)
{
	TitleTextBlock->SetText(NewText);
}