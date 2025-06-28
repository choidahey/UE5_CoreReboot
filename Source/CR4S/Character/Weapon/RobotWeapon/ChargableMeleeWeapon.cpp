// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargableMeleeWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"


// Sets default values
AChargableMeleeWeapon::AChargableMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AChargableMeleeWeapon::OnAttack()
{
	AttackPressTime=GetWorld()->GetTimeSeconds();
	if ( !OwningCharacter || !BaseInfo.AttackMontages[bIsRightHand] || !TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand]) return;
	
	Super::OnAttack();
}

void AChargableMeleeWeapon::StopAttack()
{
	if (!bCanAttack) return;
	
	Super::StopAttack();
	if (!CR4S_ENSURE(LogHong1,OwningCharacter
		&& OwningCharacter->GetMesh()
		&& BaseInfo.AttackMontages.IsValidIndex(bIsRightHand)
		&& BaseInfo.AttackMontages[bIsRightHand])
		&& TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages.IsValidIndex(bIsRightHand)
		&& TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand])
	{
		return;
	}

	UAnimInstance* AnimInstance=OwningCharacter->GetMesh()->GetAnimInstance();
	if (!CR4S_ENSURE(LogHong1,AnimInstance)) return;
	
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - AttackPressTime;

	UAnimMontage* MontageToPlay = nullptr;

	if (ElapsedTime >= TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackTimeThreshold)
	{
		MontageToPlay = TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand];

		OriginalDamageMultiplier=BaseInfo.DamageMultiplier;
		OriginalStunAmount=TypeSpecificInfo.StunAmount;

		BaseInfo.DamageMultiplier=TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackDamageMultiplier;
		TypeSpecificInfo.StunAmount=TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackStunAmount;

		bIsChargeAttacking=true;

		AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AChargableMeleeWeapon::OnChargeAttackMontageEnded);
	}
	else
	{
		MontageToPlay = BaseInfo.AttackMontages[bIsRightHand];
	}

	if (MontageToPlay)
	{
		OnMeleeAttackStarted.Broadcast(this);
		OwningCharacter->PlayAnimMontage(MontageToPlay);
	}
	StartAttackCooldown();
}

// Called when the game starts or when spawned
void AChargableMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AChargableMeleeWeapon::OnChargeAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if ( !bIsChargeAttacking
		||!TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages.IsValidIndex(bIsRightHand)
		||Montage != TypeSpecificInfo.ChargableWeaponInfo.ChargeAttackMontages[bIsRightHand])
	{
		return;
	}

	bIsChargeAttacking=true;
	
	BaseInfo.DamageMultiplier=OriginalDamageMultiplier;
	TypeSpecificInfo.StunAmount=OriginalStunAmount;

	if (CR4S_ENSURE(LogHong1,OwningCharacter
		&& OwningCharacter->GetMesh()))
	{
		return;
	}
	
	if (UAnimInstance* AnimInstance=OwningCharacter->GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this,&AChargableMeleeWeapon::OnChargeAttackMontageEnded);
	}
}

// Called every frame
void AChargableMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

