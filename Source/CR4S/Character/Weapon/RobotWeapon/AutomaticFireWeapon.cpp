// Fill out your copyright notice in the Description page of Project Settings.


#include "AutomaticFireWeapon.h"

#include "CR4S.h"
#include "Character/Weapon/Bullet/BaseBullet.h"


// Sets default values
AAutomaticFireWeapon::AAutomaticFireWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AAutomaticFireWeapon::OnAttack()
{
	if (!bCanAttack||bIsReloading) return;

	StartMuzzleFlash(TypeSpecificInfo.MuzzleSocketName);
	
	FireBulletForInterval();
	
	const float FireInterval=1.0f/TypeSpecificInfo.AutomaticFireInfo.ShotsPerSecond;
	GetWorld()->GetTimerManager().SetTimer(
		FireRateTimerHandle,
		this,
		&AAutomaticFireWeapon::FireBulletForInterval,
		FireInterval,
		true
	);
	Super::OnAttack();
}

void AAutomaticFireWeapon::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);

	StopMuzzleFlash();
	
	if (BaseInfo.AttackCooldownTime>KINDA_SMALL_NUMBER)
	{
		StartAttackCooldown();
	}
	Super::StopAttack();
}

// Called when the game starts or when spawned
void AAutomaticFireWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAutomaticFireWeapon::FireBulletForInterval()
{
	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StopAttack();
		if (!bIsReloading)
		{
			StartReload();
		}
		return;
	}

	FHitResult HitResult;
	if (!GetAimHitResult(HitResult))
	{
		StopAttack();
		return;
	}

	const FVector MuzzleLocation=GetMuzzleLocation(TypeSpecificInfo.MuzzleSocketName);
	const FVector ShootDirection=(HitResult.ImpactPoint-MuzzleLocation).GetSafeNormal();
	if (!CR4S_ENSURE(LogHong1, !ShootDirection.IsNearlyZero()
		&& TypeSpecificInfo.ProjectileClass))
	{
		StopAttack();
		return;
	}

	PlayMuzzleSFX(TypeSpecificInfo.MuzzleSocketName);
	
	const FRotator SpawnRotation=ShootDirection.Rotation();
	FireBullet(MuzzleLocation, SpawnRotation);

	AddCurrentAmmo(-1);
	ApplyRecoil();
}

// Called every frame
void AAutomaticFireWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

