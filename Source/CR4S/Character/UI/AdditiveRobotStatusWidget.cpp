// Fill out your copyright notice in the Description page of Project Settings.


#include "AdditiveRobotStatusWidget.h"
#include "CR4S.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Components/ProgressBar.h"


void UAdditiveRobotStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Energy->SetVisibility(ESlateVisibility::Hidden);
	Stun->SetVisibility(ESlateVisibility::Hidden);
}

void UAdditiveRobotStatusWidget::UpdateEnergy(const float InPercentage)
{
	Energy->SetPercent(InPercentage);
}

void UAdditiveRobotStatusWidget::UpdateStun(const float InPercentage)
{
	Stun->SetPercent(InPercentage);
}

void UAdditiveRobotStatusWidget::ToggleWidgetMode(const bool bIsRobot) const
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
}

void UAdditiveRobotStatusWidget::InitializeWidget(UBaseStatusComponent* InStatus)
{
	if (!CR4S_ENSURE(LogHong1,InStatus)) return;

	CachedStatusComponent = Cast<UModularRobotStatusComponent>(InStatus);

	if (!CR4S_ENSURE(LogHong1,CachedStatusComponent)) return;
	
	CachedStatusComponent->OnEnergyChanged.AddDynamic(this,&UAdditiveRobotStatusWidget::UpdateEnergy);
	CachedStatusComponent->OnStunChanged.AddDynamic(this,&UAdditiveRobotStatusWidget::UpdateStun);

	UpdateEnergy(CachedStatusComponent->GetCurrentEnergyPercentage());
	UpdateStun(CachedStatusComponent->GetCurrentStunPercentage());
}

void UAdditiveRobotStatusWidget::ClearBindings() const
{
	if (!CR4S_ENSURE(LogHong1,CachedStatusComponent)) return;
	CachedStatusComponent->OnEnergyChanged.RemoveAll(this);
	CachedStatusComponent->OnStunChanged.RemoveAll(this);
}
