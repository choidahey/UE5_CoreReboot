// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInfoBlockWidget.h"

#include "CR4S.h"
#include "WeaponInfoWidget.h"
#include "Character/Weapon/RobotWeapon/RangedWeapon.h"

void UWeaponInfoBlockWidget::NativeConstruct()
{
	Super::NativeConstruct();
	WeaponInfoWidgets.Add(LeftArmWeaponInfo);
	LeftArmWeaponInfo->SetSlotName(FText::FromString("LA"));
	
	WeaponInfoWidgets.Add(RightArmWeaponInfo);
	RightArmWeaponInfo->SetSlotName(FText::FromString("RA"));
	
	WeaponInfoWidgets.Add(LeftShoulderWeaponInfo);
	LeftShoulderWeaponInfo->SetSlotName(FText::FromString("LS"));
	
	WeaponInfoWidgets.Add(RightShoulderWeaponInfo);
	RightShoulderWeaponInfo->SetSlotName(FText::FromString("RS"));
	
}

void UWeaponInfoBlockWidget::InitializeWidgetBySlotIndex(ABaseWeapon* InWeapon, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1, WeaponInfoWidgets.IsValidIndex(SlotIdx) && WeaponInfoWidgets[SlotIdx])) return;

	WeaponInfoWidgets[SlotIdx]->InitializeWidgetForWeapon(InWeapon);
}

void UWeaponInfoBlockWidget::ClearBindingsToWeapon()
{
	for (int32 i=0;i<WeaponInfoWidgets.Num();i++)
	{
		if (WeaponInfoWidgets.IsValidIndex(i))
		{
			WeaponInfoWidgets[i]->ClearBindingsToWeapon();
		}
	}
}


