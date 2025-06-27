// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "Character/Characters/ModularRobot.h"
#include "Utility/DataLoaderSubsystem.h"

AMeleeWeapon::AMeleeWeapon()
{
}

void AMeleeWeapon::OnAttack()
{
	if (!bCanAttack) return;
	
	ApplySelfStun();
	AttackPressTime=GetWorld()->GetTimeSeconds();
}

void AMeleeWeapon::StopAttack()
{
	AModularRobot* Robot = GetTypedOuter<AModularRobot>();
	if (!CR4S_ENSURE(LogHong1, Robot)) return;

	const float ElapsedTime = GetWorld()->GetTimeSeconds() - AttackPressTime;

	UAnimMontage* MontageToPlay = nullptr;

	if (ElapsedTime >= TypeSpecificInfo.ChargeAttackTimeThreshold)
	{
		MontageToPlay = TypeSpecificInfo.ChargeAttackMontage;
	}
	else
	{
		MontageToPlay = BaseInfo.AttackMontage;
	}

	if (MontageToPlay)
	{
		Robot->PlayAnimMontage(MontageToPlay);
	}
	StartAttackCooldown();
	RemoveSelfStun();
}

void AMeleeWeapon::Initialize(AModularRobot* OwnerCharacter, const int32 SlotIdx)
{
	UGameInstance* GI=GetGameInstance();
	if (!GI) return;

	UDataLoaderSubsystem* DataLoader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!DataLoader||!ToolTag.IsValid()) return;

	const bool bSuccess=DataLoader->LoadWeaponInfoByTag(ToolTag,TypeSpecificInfo,BaseInfo);
	if (!CR4S_ENSURE(LogHong1,bSuccess))
	{
		return;
	}
	Super::Initialize(OwnerCharacter, SlotIdx);
}
