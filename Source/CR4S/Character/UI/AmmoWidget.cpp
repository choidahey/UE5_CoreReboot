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
}


void UAmmoWidget::InitializeWidgetForWeapon(ABaseWeapon* InWeapon, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,InWeapon)) return;

	const FGameplayTag WeaponTag=InWeapon->GetGameplayTag();
	float InitialProgress=MaxProgress;
	if (WeaponTag.MatchesTag(WeaponTags::Melee))
	{
		InitialProgress=0;
	}
	AmmoProgressWidgets[SlotIdx]->SetPercent(InitialProgress);

	ARangedWeapon* CurrentWeapon=Cast<ARangedWeapon>(InWeapon);
	if (!CR4S_ENSURE(LogHong1,CurrentWeapon)) return;

	CurrentWeapon->OnCurrentAmmoChanged.AddLambda(
	[WeakThis=TWeakObjectPtr<UAmmoWidget>(this),SlotIdx,this](const float InPercent)
	{
		if (WeakThis.IsValid())
		{
			WeakThis->UpdateCurrentAmmoWidget(SlotIdx,InPercent);
		}
	});

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
				
			}
		}
	}
}

void UAmmoWidget::UpdateCurrentAmmoWidget(const int32 SlotIdx, const float Percent)
{
	const float FinalPercent=Percent*MaxProgress;

	if (!CR4S_ENSURE(LogHong1,AmmoProgressWidgets.IsValidIndex(SlotIdx) && AmmoProgressWidgets[SlotIdx])) return;

	AmmoProgressWidgets[SlotIdx]->SetPercent(FinalPercent);
}


