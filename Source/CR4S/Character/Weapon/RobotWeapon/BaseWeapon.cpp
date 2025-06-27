// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Components/RobotInputBufferComponent.h"
#include "Character/Components/RobotWeaponComponent.h"

ABaseWeapon::ABaseWeapon()
{
	SceneComp=CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent=SceneComp;
	StaticMeshComp=CreateDefaultSubobject<UStaticMeshComponent>("SkeletalMesh");
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ABaseWeapon::OnAttack()
{
	ApplySelfStun();
	SetWeaponManagerIsDuringAttackAction(true);
}

void ABaseWeapon::StopAttack()
{
	RemoveSelfStun();
	SetWeaponManagerIsDuringAttackAction(false);
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

void ABaseWeapon::ApplySelfStun() const
{
	if (BaseInfo.bHasSelfStun)
	{
		InputBuffer->SetInputEnable(false);		
	}
}

void ABaseWeapon::RemoveSelfStun() const
{
	if (BaseInfo.bHasSelfStun)
	{
		InputBuffer->SetInputEnable(true);
	}
}

void ABaseWeapon::SetWeaponManagerIsDuringAttackAction(const bool bIsAttacking) const
{
	if (URobotWeaponComponent* WeaponManager=GetTypedOuter<URobotWeaponComponent>())
	{
		WeaponManager->SetIsDuringAttackAction(bIsAttacking);
	}
}

void ABaseWeapon::Initialize(AModularRobot* OwnerCharacter, const int32 SlotIdx)
{
	OwningCharacter=OwnerCharacter;
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;

	URobotInputBufferComponent* CurrentBuffer=OwningCharacter->FindComponentByClass<URobotInputBufferComponent>();
	if (!CR4S_ENSURE(LogHong1,CurrentBuffer)) return;
	
	InputBuffer=CurrentBuffer;
	const int32 LeftRightIndex=SlotIdx%2; // 0:Left, 1: Right
	if (!BaseInfo.SkeletalMeshs.IsValidIndex(LeftRightIndex) || BaseInfo.SkeletalMeshs[LeftRightIndex])
	StaticMeshComp->SetStaticMesh(BaseInfo.SkeletalMeshs[LeftRightIndex]);
}

int32 ABaseWeapon::GetCurrentAmmo() const
{
	return 0;
}

void ABaseWeapon::SetCurrentAmmo(const int32 NewAmmo)
{
}



