// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Character/Characters/ModularRobot.h"

UBaseWeapon::UBaseWeapon()
{
	AttackMontage=nullptr;
	OwningCharacter=nullptr;
}

void UBaseWeapon::OnAttack(const int32 WeaponIdx)
{
	if (!AttackMontage) return;

	OwningCharacter->PlayAnimMontage(AttackMontage);
}

void UBaseWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	OwningCharacter=OwnerCharacter;
}