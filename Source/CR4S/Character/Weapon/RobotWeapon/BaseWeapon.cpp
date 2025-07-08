// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Components/RobotInputBufferComponent.h"

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
	SetIsDuringAttacking(true);
}

void ABaseWeapon::StopAttack()
{
	RemoveSelfStun();
	SetIsDuringAttacking(false);
}

void ABaseWeapon::StartAttackCooldown()
{
	float ElapsedTime=GetWorld()->GetTimerManager().GetTimerElapsed(AttackCooldownTimerHandler);
	if (ElapsedTime>KINDA_SMALL_NUMBER)
	{
		return;
	}
	
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

void ABaseWeapon::SetIsDuringAttacking(const bool bIsAttacking)
{
	bIsDuringAttacking=bIsAttacking;
}

void ABaseWeapon::Initialize(AModularRobot* OwnerCharacter, const int32 SlotIdx)
{
	OwningCharacter=OwnerCharacter;
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;

	URobotInputBufferComponent* CurrentBuffer=OwningCharacter->FindComponentByClass<URobotInputBufferComponent>();
	if (!CR4S_ENSURE(LogHong1,CurrentBuffer)) return;
	
	InputBuffer=CurrentBuffer;
	
	bIsRightHand=SlotIdx%2; // 0:Left, 1: Right
	if (!BaseInfo.SkeletalMeshs.IsValidIndex(bIsRightHand) || BaseInfo.SkeletalMeshs[bIsRightHand])
	StaticMeshComp->SetStaticMesh(BaseInfo.SkeletalMeshs[bIsRightHand]);
}

int32 ABaseWeapon::GetCurrentAmmo() const
{
	return 0;
}

void ABaseWeapon::SetCurrentAmmo(const int32 NewAmmo)
{
}



