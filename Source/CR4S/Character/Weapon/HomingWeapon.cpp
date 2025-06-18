// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingWeapon.h"


// Sets default values
AHomingWeapon::AHomingWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AHomingWeapon::OnAttack()
{
	if (!bCanAttack||bIsReloading) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(LockOnTimerHandle)) return;
	
	FHitResult HitResult;
	if (!GetAimHitResult(HitResult)) return;

	AActor* TargetActor=HitResult.GetActor();

	if (TargetActor&&TargetActor!=GetOwner())
	{
		PotentialTarget=TargetActor;

		GetWorld()->GetTimerManager().SetTimer(
			LockOnTimerHandle,
			this,
			&AHomingWeapon::FireHomingBullet,
			TypeSpecificInfo.LockOnInfo.LockOnTime,
			false
		);
	}
}

void AHomingWeapon::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(LockOnTimerHandle);
	PotentialTarget=nullptr;
}

// Called when the game starts or when spawned
void AHomingWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHomingWeapon::FireHomingBullet()
{
	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StartReload();
		return;
	}
	if (!PotentialTarget.IsValid()) return;

	FireMultiBullet(PotentialTarget.Get());

	--TypeSpecificInfo.AmmoInfo.CurrentAmmo;
	ApplyRecoil();
	StartAttackCooldown();
}

// Called every frame
void AHomingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

