// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "Character/Characters/ModularRobot.h"
#include "Utility/DataLoaderSubsystem.h"

UMeleeWeapon::UMeleeWeapon()
{
}

void UMeleeWeapon::OnAttack(const int32 WeaponIdx)
{
	
}

void UMeleeWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	Super::Initialize(OwnerCharacter);

	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!GI) return;

	UDataLoaderSubsystem* DataLoader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!DataLoader||!WeaponTag.IsValid()) return;

	const bool bSuccess=DataLoader->LoadWeaponInfoByTag(WeaponTag,TypeSpecificInfo,BaseInfo);
	if (!CR4S_ENSURE(LogHong1,bSuccess))
	{
		return;
	}
}
