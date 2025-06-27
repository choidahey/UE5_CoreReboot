// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"

#include "CR4S.h"
#include "RoundProgressBar.h"
#include "Character/Weapon/RobotWeapon/BaseWeapon.h"
#include "Character/Weapon/RobotWeapon/RangedWeapon.h"

UAmmoWidget::UAmmoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AmmoProgressWidgets.Add(LeftArmCurrentAmmo);
	AmmoProgressWidgets.Add(RightArmCurrentAmmo);
	AmmoProgressWidgets.Add(LeftShoulderCurrentAmmo);
	AmmoProgressWidgets.Add(RightShoulderCurrentAmmo);

	CachedWeapons.SetNum(4);
	ReloadTimerHandles.SetNum(4);
	ReloadData.SetNum(4);
}


void UAmmoWidget::InitializeWidgetForWeapon(ABaseWeapon* InWeapon, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,InWeapon)) return;

	if (ARangedWeapon* CurrentWeapon=Cast<ARangedWeapon>(InWeapon))
	{
		const float InitialPercent=CurrentWeapon->GetCurrentAmmoPercentage();

		CurrentWeapon->OnStartReload.AddLambda(
		[WeakThis=TWeakObjectPtr<UAmmoWidget>(this),SlotIdx](const float ReloadDuration)
		{
			if (WeakThis.IsValid())
			{
				WeakThis->UpdateAmmoWidgetDuringReloading(SlotIdx, ReloadDuration);
			}	
		});
		
		UpdateCurrentAmmoWidget(SlotIdx, InitialPercent);
		CurrentWeapon->OnCurrentAmmoChanged.AddLambda(
		[WeakThis=TWeakObjectPtr<UAmmoWidget>(this),SlotIdx,this](const float InPercent)
		{
			if (WeakThis.IsValid())
			{
				WeakThis->UpdateCurrentAmmoWidget(SlotIdx,InPercent);
			}
		});
	}
	else
	{
		UpdateCurrentAmmoWidget(SlotIdx,0);
	}

	if (!CR4S_ENSURE(LogHong1,CachedWeapons.IsValidIndex(SlotIdx))) return;
	
	CachedWeapons[SlotIdx]=InWeapon;
}

void UAmmoWidget::ClearBindingsToWeapon()
{
	for (int32 i=0;i<CachedWeapons.Num();i++)
	{
		if (CachedWeapons.IsValidIndex(i))
		{
			if (ARangedWeapon* RangedWeapon=Cast<ARangedWeapon>(CachedWeapons[i]))
			{
				RangedWeapon->OnCurrentAmmoChanged.RemoveAll(this);
				RangedWeapon->OnStartReload.RemoveAll(this);
			}
		}
	}

	for (FTimerHandle& Handle : ReloadTimerHandles)
	{
		GetWorld()->GetTimerManager().ClearTimer(Handle);
	}
}

void UAmmoWidget::UpdateCurrentAmmoWidget(const int32 SlotIdx, const float Percent)
{
	const float FinalPercent=Percent*MaxProgress;

	if (!CR4S_ENSURE(LogHong1,AmmoProgressWidgets.IsValidIndex(SlotIdx) && AmmoProgressWidgets[SlotIdx])) return;

	AmmoProgressWidgets[SlotIdx]->SetPercent(FinalPercent);
}

void UAmmoWidget::UpdateAmmoWidgetDuringReloading(const int32 SlotIdx, const float ReloadDuration)
{
	if (ReloadDuration<=KINDA_SMALL_NUMBER) return;
	if (!CR4S_ENSURE(LogHong1,AmmoProgressWidgets.IsValidIndex(SlotIdx) && AmmoProgressWidgets[SlotIdx])) return;

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandles[SlotIdx]);

	ReloadData[SlotIdx].StartTime=GetWorld()->GetTimeSeconds();
	ReloadData[SlotIdx].Duration=ReloadDuration;

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("TickReloadProgress"), SlotIdx);

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandles[SlotIdx], TimerDelegate, 0.016f, true);
}

void UAmmoWidget::TickReloadProgress(int32 SlotIdx)
{
	const FReloadProgressData& CurrentReloadData=ReloadData[SlotIdx];
	const float ElapsedTime=GetWorld()->GetTimeSeconds() - CurrentReloadData.StartTime;
	const float Progress = FMath::Clamp(ElapsedTime / CurrentReloadData.Duration, 0.f, 1.f);
	const float FinalPercent=Progress*MaxProgress;

	if (AmmoProgressWidgets.IsValidIndex(SlotIdx) && AmmoProgressWidgets[SlotIdx])
	{
		AmmoProgressWidgets[SlotIdx]->SetPercent(FinalPercent);
	}
	if (Progress>=1.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandles[SlotIdx]);
	}
}


