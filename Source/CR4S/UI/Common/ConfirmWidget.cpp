#include "UI/Common/ConfirmWidget.h"
#include "Components/Button.h"

void UConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
	UE_LOG(LogTemp, Warning, TEXT("YesButtonClicked"));
}

void UConfirmWidget::OnNoButtonClicked()
{
	RemoveFromParent();
}