#include "DefaultInGameWidget.h"

#include "CharacterEnvironmentStatusWidget.h"
#include "CR4S.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Character/UI/AmmoWidget.h"
#include "Character/UI/CharacterStatusWidget.h"
#include "Character/UI/LockOnWidget.h"
#include "Character/Weapon/RobotWeapon/HomingWeapon.h"
#include "Components/Image.h"
#include "UI/Common/ProgressBarWidget.h"
#include "UI/InGame/TimeDisplayWidget.h"

void UDefaultInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AimCircle->SetVisibility(ESlateVisibility::Hidden);
	CurrentAmmoWidgets->SetVisibility(ESlateVisibility::Hidden);

	if (LockOnWidgets.IsEmpty())
	{
		LockOnWidgets.Add(LockOnWidget0);
		LockOnWidgets.Add(LockOnWidget1);
		LockOnWidgets.Add(LockOnWidget2);
		LockOnWidgets.Add(LockOnWidget3);
	}
}

void UDefaultInGameWidget::ToggleWidgetMode(const bool bIsRobot)
{
	// if (UPlayerCharacterStatusComponent* PlayerStatusComp=Cast<UPlayerCharacterStatusComponent>(InComponent))
	// {
	// 	const float Percentage=FMath::Clamp(PlayerStatusComp->GetCurrentHunger()/PlayerStatusComp->GetMaxHunger(), 0.f, 1.f);
	// 	UpdateHungerWidget(Percentage);
	// }
	
	StatusWidget->ToggleWidgetMode(bIsRobot);
	
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
}

void UDefaultInGameWidget::BindLockOnWidgetToHomingWeapon(AHomingWeapon* HomingWeapon, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,LockOnWidgets.IsValidIndex(SlotIdx)&&IsValid(HomingWeapon)&&IsValid(LockOnWidgets[SlotIdx]))) return;

	LockOnWidgets[SlotIdx]->InitializeWidgetForWeapon(HomingWeapon);
}

void UDefaultInGameWidget::UnbindAllHomingWeaponFromUI()
{
	for (ULockOnWidget* LockOnWidget : LockOnWidgets)
	{
		if (IsValid(LockOnWidget))
		{
			LockOnWidget->ClearBinding();
		}
	}
}

void UDefaultInGameWidget::BindAmmoWidgetToWeapon(ABaseWeapon* InWeapon, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,InWeapon && CurrentAmmoWidgets)) return;

	CurrentAmmoWidgets->InitializeWidgetForWeapon(InWeapon,SlotIdx);
}


void UDefaultInGameWidget::BindWidgetsToStatus(UBaseStatusComponent* InStatus)
{
	if (!CR4S_ENSURE(LogHong1,InStatus && StatusWidget)) return;
	
	StatusWidget->InitializeWidget(InStatus);
	EnvironmentStatusWidget->InitializeWidget(InStatus);

	if (!CR4S_ENSURE(LogHong1,HungerWidget)) return;
	
	if (UPlayerCharacterStatusComponent* PlayerStatusComp=Cast<UPlayerCharacterStatusComponent>(InStatus))
	{
		PlayerStatusComp->OnHungerChanged.AddUObject(this,&UDefaultInGameWidget::UpdateHungerWidget);	
	}
}



void UDefaultInGameWidget::BindEnvStatusWidgetToEnvStatus(UEnvironmentalStatusComponent* InStatus)
{
	if (!CR4S_ENSURE(LogHong1,InStatus && EnvironmentStatusWidget)) return;

	EnvironmentStatusWidget->InitializeWidget(InStatus);
}

void UDefaultInGameWidget::UnbindStatusFromUI()
{
	if (!CR4S_ENSURE(LogHong1,StatusWidget)) return;
	StatusWidget->ClearBindings();

	if (!CR4S_ENSURE(LogHong1,EnvironmentStatusWidget)) return;
	EnvironmentStatusWidget->ClearBindingsToStatusComp();
}

void UDefaultInGameWidget::UnbindEnvStatusFromUI()
{
	if (!CR4S_ENSURE(LogHong1,EnvironmentStatusWidget)) return;
	EnvironmentStatusWidget->ClearBindingsToEnvStatusComp();
}

void UDefaultInGameWidget::UnbindWeaponFromUI()
{
	if (!CR4S_ENSURE(LogHong1,CurrentAmmoWidgets)) return;
	CurrentAmmoWidgets->ClearBindingsToWeapon();
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



