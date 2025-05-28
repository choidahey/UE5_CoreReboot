#include "HelperBotInfoWidget.h"
#include "Components/TextBlock.h"

void UHelperBotInfoWidget::SetHealth(float InCurrent, float InMax)
{
	if (CurrentHealth)
	{
		CurrentHealth->SetText(FText::AsNumber(InCurrent));
	}
	if (MaxHealth)
	{
		MaxHealth->SetText(FText::AsNumber(InMax));
	}
}
