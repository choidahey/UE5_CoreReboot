// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusWidget.h"
#include "Components/ProgressBar.h"

UCharacterStatusWidget::UCharacterStatusWidget(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void UCharacterStatusWidget::UpdateHP(const float InPercentage)
{
	HP->SetPercent(InPercentage);
}

void UCharacterStatusWidget::UpdateHunger(const float InPercentage)
{
	Hunger->SetPercent(InPercentage);
}

void UCharacterStatusWidget::UpdateStamina(const float InPercentage)
{
	Stamina->SetPercent(InPercentage);
}
