#include "DefaultInGameWidget.h"

#include "CR4S.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Character/UI/CharacterStatusWidget.h"
#include "Character/UI/RoundProgressBar.h"
#include "Character/Weapon/RobotWeapon/HomingWeapon.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "UI/Common/ProgressBarWidget.h"
#include "UI/InGame/TimeDisplayWidget.h"

void UDefaultInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	
	LockOnImage->SetColorAndOpacity(FLinearColor::White);
	LockOnImage->SetVisibility(ESlateVisibility::Hidden);
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

void UDefaultInGameWidget::BindWidgetToHomingWeapon(AHomingWeapon* HomingWeapon)
{
	if (!CR4S_ENSURE(LogHong1,HomingWeapon)) return;

	HomingWeapon->OnLockOnStarted.AddDynamic(this,&UDefaultInGameWidget::UpdateImageVisible);
	HomingWeapon->OnLockOnCanceled.AddDynamic(this,&UDefaultInGameWidget::UpdateImageInvisible);
	HomingWeapon->OnLockOnFinished.AddDynamic(this,&UDefaultInGameWidget::SetLockedOnColor);
	HomingWeapon->OnTryingToLockOn.AddDynamic(this,&UDefaultInGameWidget::UpdateImagePosition);
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

void UDefaultInGameWidget::UpdateImageVisible()
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	LockOnImage->SetColorAndOpacity(FLinearColor::White);
	LockOnImage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UDefaultInGameWidget::UpdateImageInvisible()
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	LockOnImage->SetVisibility(ESlateVisibility::Hidden);
}

void UDefaultInGameWidget::SetLockedOnColor()
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	LockOnImage->SetColorAndOpacity(FLinearColor::Red);
}

void UDefaultInGameWidget::UpdateImagePosition(const FVector2D& NewPosition)
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;

	if (UCanvasPanelSlot* ImageSlot=UWidgetLayoutLibrary::SlotAsCanvasSlot(LockOnImage))
	{
		const float ViewportScale=UWidgetLayoutLibrary::GetViewportScale(this);
		FVector2D LocalPosition=NewPosition/ViewportScale;

		ImageSlot->SetAlignment({0.5f,0.5f});
		ImageSlot->SetPosition(LocalPosition);
		
	}
}


