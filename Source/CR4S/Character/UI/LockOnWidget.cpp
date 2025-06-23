// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnWidget.h"
#include "CR4S.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/Weapon/RobotWeapon/HomingWeapon.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

class UCanvasPanelSlot;

ULockOnWidget::ULockOnWidget(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void ULockOnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	LockOnImage->SetVisibility(ESlateVisibility::Hidden);
}

void ULockOnWidget::InitializeWidgetForWeapon(AHomingWeapon* HomingWeapon)
{
	if (!CR4S_ENSURE(LogHong1,HomingWeapon)) return;
	
	HomingWeapon->OnLockOnStarted.AddDynamic(this,&ULockOnWidget::UpdateImageVisible);
	HomingWeapon->OnLockOnCanceled.AddDynamic(this,&ULockOnWidget::UpdateImageInvisible);
	HomingWeapon->OnLockOnFinished.AddDynamic(this,&ULockOnWidget::SetLockedOnColor);
	HomingWeapon->OnTryingToLockOn.AddDynamic(this,&ULockOnWidget::UpdateImagePosition);
}

void ULockOnWidget::UpdateImageVisible()
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	LockOnImage->SetColorAndOpacity(FLinearColor::White);
	LockOnImage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void ULockOnWidget::UpdateImageInvisible()
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	LockOnImage->SetVisibility(ESlateVisibility::Hidden);
}

void ULockOnWidget::SetLockedOnColor()
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;
	LockOnImage->SetColorAndOpacity(FLinearColor::Red);
}

void ULockOnWidget::UpdateImagePosition(const FVector2D& NewPosition)
{
	if (!CR4S_ENSURE(LogHong1,LockOnImage)) return;

	if (UCanvasPanelSlot* ImageSlot=UWidgetLayoutLibrary::SlotAsCanvasSlot(this))
	{
		const float ViewportScale=UWidgetLayoutLibrary::GetViewportScale(this);
		FVector2D LocalPosition=NewPosition/ViewportScale;

		ImageSlot->SetAlignment({0.5f,0.5f});
		ImageSlot->SetPosition(LocalPosition);
	}
}