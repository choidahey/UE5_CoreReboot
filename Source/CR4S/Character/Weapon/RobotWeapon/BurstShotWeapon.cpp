// Fill out your copyright notice in the Description page of Project Settings.


#include "BurstShotWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/Bullet/BaseBullet.h"


// Sets default values
ABurstShotWeapon::ABurstShotWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABurstShotWeapon::OnAttack()
{
	if (!bCanAttack || bIsReloading) return;

	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StartReload();
		return;
	}
	
	StartAttackCooldown();

	StartSequentialFire();

	Super::OnAttack();
}

// Called when the game starts or when spawned
void ABurstShotWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABurstShotWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

