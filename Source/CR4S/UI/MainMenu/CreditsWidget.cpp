#include "UI/MainMenu/CreditsWidget.h"
#include "Components/Button.h"

void UCreditsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExitButton)
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &UCreditsWidget::OnExitButtonClicked);
	}
}

void UCreditsWidget::OnExitButtonClicked()
{
	RemoveFromParent();
}