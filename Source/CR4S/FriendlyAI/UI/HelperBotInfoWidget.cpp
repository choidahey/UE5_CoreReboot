#include "HelperBotInfoWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FriendlyAI/BaseHelperBot.h"

void UHelperBotInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, 
		&UHelperBotInfoWidget::UpdateWidgets, 0.1f, true);
}

void UHelperBotInfoWidget::UpdateWidgets()
{
	if (IsValid(OwnerHelperBot))
	{
		if (CurrentHPTextBlock)
			CurrentHPTextBlock->SetText(GetCurrentHealthText());
		if (MaxHPTextBlock)
			MaxHPTextBlock->SetText(GetMaxHealthText());
		if (ProgressBar)
			ProgressBar->SetPercent(GetHealthPercent());
	}
}

FText UHelperBotInfoWidget::GetCurrentHealthText()
{
	if (IsValid(OwnerHelperBot))
	{
		return FText::AsNumber((int32)OwnerHelperBot->GetCurrentHealth());
	}
	return FText::FromString("0");
}

FText UHelperBotInfoWidget::GetMaxHealthText()
{
	if (IsValid(OwnerHelperBot))
	{
		return FText::AsNumber((int32)OwnerHelperBot->GetMaxHealth());
	}
	return FText::FromString("0");
}

float UHelperBotInfoWidget::GetHealthPercent()
{
	if (IsValid(OwnerHelperBot))
	{
		float MaxHealth = OwnerHelperBot->GetMaxHealth();
		if (MaxHealth > 0)
		{
			return OwnerHelperBot->GetCurrentHealth() / MaxHealth;
		}
	}
	return 0.0f;
}