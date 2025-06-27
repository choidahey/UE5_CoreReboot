// Fill out your copyright notice in the Description page of Project Settings.


#include "SustainedMeleeWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"


// Sets default values
ASustainedMeleeWeapon::ASustainedMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASustainedMeleeWeapon::OnAttack()
{
	if (!bCanAttack || !OwningCharacter || !BaseInfo.AttackMontage) return;
	ApplySelfStun();

	
	OwningCharacter->PlayAnimMontage(BaseInfo.AttackMontage);
}

void ASustainedMeleeWeapon::StopAttack()
{
	RemoveSelfStun();
	StartAttackCooldown();

	if (!CR4S_ENSURE(LogHong1,OwningCharacter && !BaseInfo.AttackMontage)) return;

	OwningCharacter->StopAnimMontage(BaseInfo.AttackMontage);
}

// Called when the game starts or when spawned
void ASustainedMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASustainedMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

