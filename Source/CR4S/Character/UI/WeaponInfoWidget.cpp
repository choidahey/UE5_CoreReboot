// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInfoWidget.h"

#include "CR4S.h"
#include "Character/Weapon/RobotWeapon/RangedWeapon.h"
#include "Components/TextBlock.h"

void UWeaponInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWeaponInfoWidget::InitializeWidgetForWeapon(ABaseWeapon* InWeapon)
{
	if (!CR4S_ENSURE(LogHong1,InWeapon)) return;

	if (ARangedWeapon* RangedWeapon=Cast<ARangedWeapon>(InWeapon))
	{
		const int32 CurrentAmmo=RangedWeapon->GetCurrentAmmo();
		const int32 MaxAmmo=RangedWeapon->GetMaxAmmo();

		UpdateCurrentAmmoCount(CurrentAmmo);
		UpdateMaxAmmoCount(MaxAmmo);
		
		RangedWeapon->OnCurrentAmmoCountChanged.AddDynamic(this,&UWeaponInfoWidget::UpdateCurrentAmmoCount);
		RangedWeapon->OnMaxAmmoCountChanged.AddDynamic(this,&UWeaponInfoWidget::UpdateMaxAmmoCount);
		RangedWeapon->OnStartReload.AddUObject(this,&UWeaponInfoWidget::StartReload);

		CachedWeapon=RangedWeapon;
	}
}

void UWeaponInfoWidget::ClearBindingsToWeapon()
{
	if (CachedWeapon.IsValid())
	{
		if (ARangedWeapon* RangedWeapon=Cast<ARangedWeapon>(CachedWeapon))
		{
			RangedWeapon->OnCurrentAmmoCountChanged.RemoveAll(this);
			RangedWeapon->OnMaxAmmoCountChanged.RemoveAll(this);
			RangedWeapon->OnStartReload.RemoveAll(this);
		}
		UpdateCurrentAmmoCount(-1);
		UpdateMaxAmmoCount(-1);
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void UWeaponInfoWidget::UpdateCurrentAmmoCount(const int32 CurrentAmmoCount)
{
	const FText Text = CurrentAmmoCount==-1 ? FText::FromString("-") : FText::AsNumber(CurrentAmmoCount);
	CurrentAmmoCountWidget->SetText(Text);
}

void UWeaponInfoWidget::UpdateMaxAmmoCount(const int32 MaxAmmoCount)
{
	const FText Text = MaxAmmoCount==-1 ? FText::FromString("-") : FText::AsNumber(MaxAmmoCount);
	MaxAmmoCountWidget->SetText(Text);
}

void UWeaponInfoWidget::SetSlotName(const FText SlotName)
{
	SlotNameWidget->SetText(SlotName);
}

void UWeaponInfoWidget::StartReload(const float ReloadDuration)
{
	if (ReloadDuration<=KINDA_SMALL_NUMBER) return;

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	
	CurrentAmmoCountWidget->SetText(FText::FromString("Reload"));
	CurrentAmmoCountWidget->SetColorAndOpacity(FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UWeaponInfoWidget::StopReload,
		ReloadDuration,
		false
	);
}

void UWeaponInfoWidget::StopReload()
{
	CurrentAmmoCountWidget->SetColorAndOpacity(FLinearColor::White);
}

