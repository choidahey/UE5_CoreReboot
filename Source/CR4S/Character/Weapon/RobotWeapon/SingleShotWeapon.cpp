// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleShotWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/Bullet/BaseBullet.h"


// Sets default values
ASingleShotWeapon::ASingleShotWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ASingleShotWeapon::OnAttack()
{
	if (!bCanAttack || bIsReloading) return;

	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StartReload();
		return;
	}
	
	FHitResult HitResult;
	if (!GetAimHitResult(HitResult)) return;
	
	const FVector MuzzleLocation=GetMuzzleLocation(TypeSpecificInfo.MuzzleSocketName);
	const FVector ShootDirection=(HitResult.ImpactPoint-MuzzleLocation).GetSafeNormal();
	if (!CR4S_ENSURE(LogHong1,!ShootDirection.IsNearlyZero()
		&& TypeSpecificInfo.ProjectileClass))
	{
		return;
	}

	PlayMuzzleVFX(TypeSpecificInfo.MuzzleSocketName);
	PlayMuzzleSFX(TypeSpecificInfo.MuzzleSocketName);
	
	const FRotator SpawnRotation=ShootDirection.Rotation();
	FireBullet(MuzzleLocation,SpawnRotation);
	
	AddCurrentAmmo(-1);
	ApplyRecoil();
	StartAttackCooldown();
	Super::OnAttack();
}

void ASingleShotWeapon::StopAttack()
{
	Super::StopAttack();
	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StartReload();
	}
}

// Called when the game starts or when spawned
void ASingleShotWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASingleShotWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

