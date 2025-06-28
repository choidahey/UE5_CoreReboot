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
	if ( !OwningCharacter || !BaseInfo.AttackMontages[bIsRightHand] || !TypeSpecificInfo.ChargeAttackMontages[bIsRightHand]) return;
	
	Super::OnAttack();
}

void AChargableMeleeWeapon::StopAttack()
{
	if (!bCanAttack) return;
	
	Super::StopAttack();
	if (!CR4S_ENSURE(LogHong1,OwningCharacter
		&& BaseInfo.AttackMontages.IsValidIndex(bIsRightHand)
		&& BaseInfo.AttackMontages[bIsRightHand])
		&& TypeSpecificInfo.ChargeAttackMontages.IsValidIndex(bIsRightHand)
		&& TypeSpecificInfo.ChargeAttackMontages[bIsRightHand])
	{
		return;
	}

	const float ElapsedTime = GetWorld()->GetTimeSeconds() - AttackPressTime;

	UAnimMontage* MontageToPlay = nullptr;

	if (ElapsedTime >= TypeSpecificInfo.ChargeAttackTimeThreshold)
	{
		MontageToPlay = TypeSpecificInfo.ChargeAttackMontages[bIsRightHand];
	}
	else
	{
		MontageToPlay = BaseInfo.AttackMontages[bIsRightHand];
	}

	if (MontageToPlay)
	{
		OwningCharacter->PlayAnimMontage(MontageToPlay);
	}
	StartAttackCooldown();
	Super::StopAttack();
}

// Called when the game starts or when spawned
void AChargableMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChargableMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

