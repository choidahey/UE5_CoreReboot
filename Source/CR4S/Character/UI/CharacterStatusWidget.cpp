#include "CharacterStatusWidget.h"

#include "CR4S.h"
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

void UCharacterStatusWidget::ToggleWidgetMode(const bool bIsRobot) const
{
	if (Resource)
	{
		Resource->SetFillColorAndOpacity(bIsRobot
			? FColor::Red
			: FColor::Yellow);
	}
}

void UCharacterStatusWidget::InitializeWidget(UBaseStatusComponent* InStatus)
{
	if (!CR4S_ENSURE(LogHong1,InStatus)) return;

	CachedStatusComponent = InStatus;
	
	InStatus->OnHPChanged.AddUObject(this,&UCharacterStatusWidget::UpdateHP);
	InStatus->OnResourceChanged.AddUObject(this,&UCharacterStatusWidget::UpdateResource);

	UpdateHP(InStatus->GetCurrentHPPercentage());
	UpdateResource(InStatus->GetCurrentResourcePercentage());
}

void UCharacterStatusWidget::ClearBindings() const
{
	if (!CR4S_ENSURE(LogHong1,CachedStatusComponent)) return;
	CachedStatusComponent->OnHPChanged.RemoveAll(this);
	CachedStatusComponent->OnResourceChanged.RemoveAll(this);
}
