// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Components/RobotCombatComponent.h"
#include "Utility/DataLoaderSubsystem.h"

UMeleeWeapon::UMeleeWeapon()
{
}

void UMeleeWeapon::OnAttack()
{
	URobotCombatComponent* Comp=GetTypedOuter<URobotCombatComponent>();
	if (!CR4S_ENSURE(LogHong1,Comp)) return;
	
	Comp->SetTopSocketName(TypeSpecificInfo.TopSocketName);
	Comp->SetBottomSocketName(TypeSpecificInfo.BottomSocketName);
	
	AModularRobot* Robot=GetTypedOuter<AModularRobot>();
	if (!CR4S_ENSURE(LogHong1,Robot)||!CR4S_ENSURE(LogHong1,BaseInfo.AttackMontage)) return;
	Robot->PlayAnimMontage(BaseInfo.AttackMontage);
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
