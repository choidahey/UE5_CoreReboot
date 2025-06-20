// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Components/RobotInputBufferComponent.h"

ABaseWeapon::ABaseWeapon()
{
	SceneComp=CreateDefaultSubobject<USceneComponent>(FName("Root"));
	RootComponent=SceneComp;
	SkeletalMeshComp=CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMeshComp->SetupAttachment(RootComponent);
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
}

void ABaseWeapon::StopAttack()
{
	RemoveSelfStun();
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

void ABaseWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	OwningCharacter=OwnerCharacter;
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;

	URobotInputBufferComponent* CurrentBuffer=OwningCharacter->FindComponentByClass<URobotInputBufferComponent>();
	if (!CR4S_ENSURE(LogHong1,CurrentBuffer)) return;
	
	InputBuffer=CurrentBuffer;
	//SkeletalMeshComp->SetSkeletalMesh(BaseInfo.SkeletalMesh);
}


