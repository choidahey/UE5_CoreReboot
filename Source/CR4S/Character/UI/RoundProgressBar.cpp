// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundProgressBar.h"
#include "CR4S.h"
#include "Character/Weapon/RobotWeapon/HomingWeapon.h"
#include "Components/Image.h"

URoundProgressBar::URoundProgressBar(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void URoundProgressBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void URoundProgressBar::SetPercent(const float InPercent)
{
	if (!IsValid(DynamicMaterialInstance))
	{
		if (!IsValid(ParentMaterial))
		{
			return;
		}
		if (!IsValid(RoundImage))
		{
			return;
		}
		DynamicMaterialInstance=UMaterialInstanceDynamic::Create(ParentMaterial,this);
		RoundImage->SetBrushFromMaterial(DynamicMaterialInstance);
	}

	if (IsValid(DynamicMaterialInstance))
	{
		DynamicMaterialInstance->SetScalarParameterValue("Percent",InPercent);
	}
}
