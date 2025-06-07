#include "DefaultInGameWidget.h"

#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Character/UI/CharacterStatusWidget.h"
#include "UI/Common/ProgressBarWidget.h"
#include "UI/InGame/TimeDisplayWidget.h"

void UDefaultInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDefaultInGameWidget::InitializeStatusWidget(UBaseStatusComponent* InComponent, bool bIsRobot)
{
	float Percentage=FMath::Clamp(InComponent->GetCurrentHP()/InComponent->GetMaxHP(), 0.f, 1.f);
	UpdateHPWidget(Percentage);
	
	Percentage=FMath::Clamp(InComponent->GetCurrentResource()/InComponent->GetMaxResource(), 0.f, 1.f);
	UpdateResourceWidget(Percentage);

	if (UPlayerCharacterStatusComponent* PlayerStatusComp=Cast<UPlayerCharacterStatusComponent>(InComponent))
	{
		Percentage=FMath::Clamp(PlayerStatusComp->GetCurrentHunger()/PlayerStatusComp->GetMaxHunger(), 0.f, 1.f);
		UpdateHungerWidget(Percentage);
	}
	if (UModularRobotStatusComponent* RobotStatusComp=Cast<UModularRobotStatusComponent>(InComponent))
	{
		Percentage=FMath::Clamp(RobotStatusComp->GetCurrentEnergy()/RobotStatusComp->GetMaxEnergy(), 0.f, 1.f);
		UpdateEnergyWidget(Percentage);

		Percentage=FMath::Clamp(RobotStatusComp->GetCurrentStun()/RobotStatusComp->GetMaxStun(), 0.f, 1.f);
		UpdateStunWidget(Percentage);
	}
	StatusWidget->ToggleWidgetMode(bIsRobot);
}

void UDefaultInGameWidget::UpdateHPWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateHP(InPercentage);
	}
}

void UDefaultInGameWidget::UpdateResourceWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateResource(InPercentage);
	}
}

void UDefaultInGameWidget::UpdateEnergyWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateEnergy(InPercentage);
	}
}


void UDefaultInGameWidget::UpdateStunWidget(const float InPercentage)
{
	if (StatusWidget)
	{
		StatusWidget->UpdateStun(InPercentage);
	}
}

void UDefaultInGameWidget::UpdateHungerWidget(const float InPercentage)
{
	if (HungerWidget)
	{
		HungerWidget->SetPercent(InPercentage);
	}
}


void UDefaultInGameWidget::UpdateTimeWidget(FWorldTimeData CurrentTimeData)
{
	if (TimeDisplayWidget)
	{
		TimeDisplayWidget->UpdateTime(CurrentTimeData.Day, CurrentTimeData.Minute, CurrentTimeData.Second);
	}
}

