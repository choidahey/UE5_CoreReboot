// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"

#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Character/UI/CharacterStatusWidget.h"

void UInGameWidget::InitializeWidget(UPlayerCharacterStatusComponent* InComponent)
{
	float Percentage=FMath::Clamp(static_cast<float>(InComponent->GetMaxHP())/InComponent->GetCurrentHP(), 0.f, 1.f);
	UpdateHPWidget(Percentage);
	
	Percentage=FMath::Clamp(static_cast<float>(InComponent->GetMaxHunger())/InComponent->GetCurrentHunger(), 0.f, 1.f);
	UpdateHungerWidget(Percentage);
	
	Percentage=FMath::Clamp(static_cast<float>(InComponent->GetMaxStamina())/InComponent->GetCurrentStamina(), 0.f, 1.f);
	UpdateStaminaWidget(Percentage);
}

void UInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInGameWidget::UpdateHPWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateHP(InPercentage);
	}
}

void UInGameWidget::UpdateHungerWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateHunger(InPercentage);
	}
}

void UInGameWidget::UpdateStaminaWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateStamina(InPercentage);
	}
}

