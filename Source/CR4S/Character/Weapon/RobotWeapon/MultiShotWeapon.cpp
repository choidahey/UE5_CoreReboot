// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiShotWeapon.h"

#include "CR4S.h"


// Sets default values
AMultiShotWeapon::AMultiShotWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AMultiShotWeapon::OnAttack()
{
	if (!bCanAttack || bIsReloading) return;

	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StartReload();
		return;
	}
	
	FireMultiBullet();
	
	--TypeSpecificInfo.AmmoInfo.CurrentAmmo;
	ApplyRecoil();
	StartAttackCooldown();
}

// Called when the game starts or when spawned
void AMultiShotWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMultiShotWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

