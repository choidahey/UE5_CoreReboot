// Fill out your copyright notice in the Description page of Project Settings.


#include "SustainableMeleeWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"


// Sets default values
ASustainableMeleeWeapon::ASustainableMeleeWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASustainableMeleeWeapon::OnAttack()
{
	if (!bCanAttack || !OwningCharacter || !BaseInfo.AttackMontage) return;
	
	OwningCharacter->PlayAnimMontage(BaseInfo.AttackMontage);
	Super::OnAttack();
}

void ASustainableMeleeWeapon::StopAttack()
{
	Super::StopAttack();

	if (!CR4S_ENSURE(LogHong1,OwningCharacter && !BaseInfo.AttackMontage)) return;

	OwningCharacter->StopAnimMontage(BaseInfo.AttackMontage);
}

// Called when the game starts or when spawned
void ASustainableMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASustainableMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

