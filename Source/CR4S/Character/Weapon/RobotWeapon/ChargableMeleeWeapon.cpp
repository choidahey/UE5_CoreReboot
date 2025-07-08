// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargableMeleeWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"


// Sets default values
AChargableMeleeWeapon::AChargableMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AChargableMeleeWeapon::OnAttack()
{
	if (!bCanAttack || !OwningCharacter || !BaseInfo.AttackMontages[bIsRightHand] || !TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand]) return;
	
	AttackPressTime=GetWorld()->GetTimeSeconds();
	ApplySelfStun();
}

void AChargableMeleeWeapon::StopAttack()
{
	if (!bCanAttack) return;
	
	if (!CR4S_ENSURE(LogHong1,
		OwningCharacter
		&& OwningCharacter->GetMesh()
		&& BaseInfo.AttackMontages.IsValidIndex(bIsRightHand)
		&& BaseInfo.AttackMontages[bIsRightHand]
		&& TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages.IsValidIndex(bIsRightHand)
		&& TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand]))
	{
		return;
	}

	RemoveSelfStun();
	
	UAnimInstance* AnimInstance=OwningCharacter->GetMesh()->GetAnimInstance();
	if (!CR4S_ENSURE(LogHong1,AnimInstance)) return;
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AChargableMeleeWeapon::OnAttackMontageEnded);
	
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - AttackPressTime;

	UAnimMontage* MontageToPlay = nullptr;

	if (ElapsedTime >= TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackTimeThreshold)
	{
		MontageToPlay = TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand];

		OriginalDamageMultiplier=BaseInfo.DamageMultiplier;
		OriginalStunAmount=TypeSpecificInfo.StunAmount;

		BaseInfo.DamageMultiplier=TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackDamageMultiplier;
		TypeSpecificInfo.StunAmount=TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackStunAmount;
	}
	else
	{
		MontageToPlay = BaseInfo.AttackMontages[bIsRightHand];
	}

	if (MontageToPlay)
	{
		OnMeleeAttackStarted.Broadcast(this);
		OwningCharacter->PlayAnimMontage(MontageToPlay);
		SetIsDuringAttacking(true);
	}
}

// Called when the game starts or when spawned
void AChargableMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AChargableMeleeWeapon::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CR4S_ENSURE(LogHong1,OwningCharacter
		&& OwningCharacter->GetMesh())
		&& Montage)
	{
		return;
	}

	SetIsDuringAttacking(false);
	StartAttackCooldown();
	//Reset Attack Pressed Time
	AttackPressTime=GetWorld()->GetTimeSeconds();
	
	if (TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages.IsValidIndex(bIsRightHand)
		&& TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand] == Montage)
	{
		BaseInfo.DamageMultiplier=OriginalDamageMultiplier;
		TypeSpecificInfo.StunAmount=OriginalStunAmount;
	}
	
	if (UAnimInstance* AnimInstance=OwningCharacter->GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this,&AChargableMeleeWeapon::OnAttackMontageEnded);
	}
}

// Called every frame
void AChargableMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


