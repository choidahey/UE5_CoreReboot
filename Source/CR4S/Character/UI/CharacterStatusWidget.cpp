#include "CharacterStatusWidget.h"

#include "Character/Components/BaseStatusComponent.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "UI/Common/ProgressBarWidget.h"

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

void UCharacterStatusWidget::ToggleWidgetMode(UBaseStatusComponent* InComponent, const bool bIsRobot)
{
	float Percentage=FMath::Clamp(InComponent->GetCurrentHP()/InComponent->GetMaxHP(), 0.f, 1.f);
	UpdateHP(Percentage);
	
	Percentage=FMath::Clamp(InComponent->GetCurrentResource()/InComponent->GetMaxResource(), 0.f, 1.f);
	UpdateResource(Percentage);
	
	if (UModularRobotStatusComponent* RobotStatusComp=Cast<UModularRobotStatusComponent>(InComponent))
	{
		Percentage=FMath::Clamp(RobotStatusComp->GetCurrentEnergy()/RobotStatusComp->GetMaxEnergy(), 0.f, 1.f);
		UpdateEnergy(Percentage);

		Percentage=FMath::Clamp(RobotStatusComp->GetCurrentStun()/RobotStatusComp->GetMaxStun(), 0.f, 1.f);
		UpdateStun(Percentage);
	}
	
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
