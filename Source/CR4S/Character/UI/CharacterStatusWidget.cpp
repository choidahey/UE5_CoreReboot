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

void UCharacterStatusWidget::UpdateResource(const float InPercentage)
{
	Resource->SetPercent(InPercentage);
}

void UCharacterStatusWidget::UpdateEnergy(const float InPercentage)
{
	Energy->SetPercent(InPercentage);
}


void UCharacterStatusWidget::UpdateStun(const float InPercentage)
{
	Stun->SetPercent(InPercentage);
}

void UCharacterStatusWidget::ToggleWidgetMode(const bool bIsRobot)
{
	if (Energy)
	{
		Energy->SetVisibility((bIsRobot
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden));
	}
	if (Stun)
	{
		Stun->SetVisibility((bIsRobot
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden));
	}
	if (Resource)
	{
		Resource->SetFillColorAndOpacity(bIsRobot
			? FColor::Red
			: FColor::Yellow);
	}
}
