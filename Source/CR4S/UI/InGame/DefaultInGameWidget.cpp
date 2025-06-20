#include "DefaultInGameWidget.h"

#include "CR4S.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Character/UI/AmmoWidget.h"
#include "Character/UI/CharacterStatusWidget.h"
#include "Character/UI/LockOnWidget.h"
#include "Character/Weapon/RobotWeapon/HomingWeapon.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "UI/Common/ProgressBarWidget.h"
#include "UI/InGame/TimeDisplayWidget.h"

void UDefaultInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (!CR4S_ENSURE(LogHong1,LockOnWidget)) return;
	
	AimCircle->SetVisibility(ESlateVisibility::Hidden);

	CurrentAmmoWidgets->SetVisibility(ESlateVisibility::Hidden);
}

void UDefaultInGameWidget::ToggleWidgetMode(UBaseStatusComponent* InComponent, bool bIsRobot)
{
	if (UPlayerCharacterStatusComponent* PlayerStatusComp=Cast<UPlayerCharacterStatusComponent>(InComponent))
	{
		const float Percentage=FMath::Clamp(PlayerStatusComp->GetCurrentHunger()/PlayerStatusComp->GetMaxHunger(), 0.f, 1.f);
		UpdateHungerWidget(Percentage);
	}
	
	StatusWidget->ToggleWidgetMode(InComponent,bIsRobot);
	
	if (!CR4S_ENSURE(LogHong1,CrosshairWidget && AimCircle)) return;
	
	if (bIsRobot)
	{
		CrosshairWidget->SetBrushFromTexture(RobotCrosshair);
		AimCircle->SetVisibility(ESlateVisibility::HitTestInvisible);
		CurrentAmmoWidgets->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		CrosshairWidget->SetBrushFromTexture(DefaultCrosshair);
		AimCircle->SetVisibility(ESlateVisibility::Hidden);
		CurrentAmmoWidgets->SetVisibility(ESlateVisibility::Hidden);
	}

	if (!CR4S_ENSURE(LogHong1,CurrentAmmoWidgets)) return;
}

void UDefaultInGameWidget::BindWidgetToHomingWeapon(AHomingWeapon* HomingWeapon)
{
	if (!CR4S_ENSURE(LogHong1,HomingWeapon && LockOnWidget)) return;

	LockOnWidget->InitializeWidgetForWeapon(HomingWeapon);	
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



