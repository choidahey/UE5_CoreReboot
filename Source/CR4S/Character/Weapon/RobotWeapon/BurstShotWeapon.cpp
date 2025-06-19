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

	BurstShotsRemaining=TypeSpecificInfo.BurstShotInfo.ShotsPerBurst;

	GetWorld()->GetTimerManager().SetTimer(
		BurstTimerHandle,
		this,
		&ABurstShotWeapon::FireBurstShot,
		TypeSpecificInfo.BurstShotInfo.TimeBetweenShots,
		true
	);
}

// Called when the game starts or when spawned
void ABurstShotWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABurstShotWeapon::FireBurstShot()
{
	if (BurstShotsRemaining<=0 || TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
		{
			StartReload();
		}
		return;
	}

	FHitResult HitResult;
	if (!GetAimHitResult(HitResult)) return;
	
	const FVector MuzzleLocation=GetMuzzleLocation(TypeSpecificInfo.MuzzleSocketName);
	const FVector ShootDirection= (HitResult.ImpactPoint-MuzzleLocation).GetSafeNormal();
	if (!CR4S_ENSURE(LogHong1,!ShootDirection.IsNearlyZero()
		&& TypeSpecificInfo.ProjectileClass))
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		return;
	}

	const FRotator SpawnRotation=ShootDirection.Rotation();
	
	FireBullet(MuzzleLocation,SpawnRotation);
	
	--TypeSpecificInfo.AmmoInfo.CurrentAmmo;
	--BurstShotsRemaining;
	ApplyRecoil();
}

// Called every frame
void ABurstShotWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

