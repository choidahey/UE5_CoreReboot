#include "UI/MainMenu/SettingsWidget.h"
#include "Components/Button.h"
void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExitButton)
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &USettingsWidget::OnExitButtonClicked);
	}
}

void USettingsWidget::OnExitButtonClicked()
{
	RemoveFromViewport();
}