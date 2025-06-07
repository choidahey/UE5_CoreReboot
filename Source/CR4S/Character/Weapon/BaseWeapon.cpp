// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Character/Characters/ModularRobot.h"

UBaseWeapon::UBaseWeapon()
{
}

void UBaseWeapon::OnAttack()
{
}

float UBaseWeapon::ComputeFinalDamage()
{
	float FinalDamage=0;
	if (UModularRobotStatusComponent* StatusComp=OwningCharacter->FindComponentByClass<UModularRobotStatusComponent>())
	{
		return FinalDamage=StatusComp->GetAttackPower()*StatusComp->GetAttackPowerMultiplier()*BaseInfo.DamageMultiplier;
	}
	return FinalDamage;
}

void UBaseWeapon::StartAttackCooldown()
{
	bCanAttack=false;

	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimerHandler,
		this,
		&UBaseWeapon::ResetAttackCooldown,
		BaseInfo.AttackCooldownTime,
		false
	);
}

void UBaseWeapon::ResetAttackCooldown()
{
	bCanAttack=true;
}

void UBaseWeapon::SetGameplayTag(const FGameplayTag GameplayTag)
{
	WeaponTag=GameplayTag;
}

void UBaseWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	OwningCharacter=OwnerCharacter;
}

