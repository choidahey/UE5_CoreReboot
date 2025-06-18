// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Character/Characters/ModularRobot.h"

ABaseWeapon::ABaseWeapon()
{
	SkeletalMeshComp=CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	RootComponent=SkeletalMeshComp;
}

float ABaseWeapon::ComputeFinalDamage()
{
	float FinalDamage=0;
	if (UModularRobotStatusComponent* StatusComp=OwningCharacter->FindComponentByClass<UModularRobotStatusComponent>())
	{
		return FinalDamage=StatusComp->GetAttackPower()*StatusComp->GetAttackPowerMultiplier()*BaseInfo.DamageMultiplier;
	}
	return FinalDamage;
}

void ABaseWeapon::StartAttackCooldown()
{
	bCanAttack=false;

	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimerHandler,
		this,
		&ABaseWeapon::ResetAttackCooldown,
		BaseInfo.AttackCooldownTime,
		false
	);
}

void ABaseWeapon::ResetAttackCooldown()
{
	bCanAttack=true;
}

void ABaseWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	OwningCharacter=OwnerCharacter;
	SkeletalMeshComp->SetSkeletalMesh(BaseInfo.SkeletalMesh);
}

void ABaseWeapon::StopAttack()
{
}

