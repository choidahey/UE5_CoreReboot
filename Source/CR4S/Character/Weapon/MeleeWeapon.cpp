// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "Character/Characters/ModularRobot.h"
#include "Utility/DataLoaderSubsystem.h"

AMeleeWeapon::AMeleeWeapon()
{
}

void AMeleeWeapon::OnAttack()
{
	AModularRobot* Robot=GetTypedOuter<AModularRobot>();
	if (!CR4S_ENSURE(LogHong1,Robot)||!CR4S_ENSURE(LogHong1,BaseInfo.AttackMontage)) return;
	Robot->PlayAnimMontage(BaseInfo.AttackMontage);
}

void AMeleeWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	Super::Initialize(OwnerCharacter);
	if (!CR4S_ENSURE(LogHong1,OwnerCharacter)) return;
	
	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!GI) return;

	UDataLoaderSubsystem* DataLoader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!DataLoader||!ToolTag.IsValid()) return;

	const bool bSuccess=DataLoader->LoadWeaponInfoByTag(ToolTag,TypeSpecificInfo,BaseInfo);
	if (!CR4S_ENSURE(LogHong1,bSuccess))
	{
		return;
	}
}
