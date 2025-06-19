// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"


// Sets default values
AHomingWeapon::AHomingWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AHomingWeapon::OnAttack()
{
	if (!bCanAttack||bIsReloading||bIsTryingToLockOn) return;
	
	if (TypeSpecificInfo.AmmoInfo.CurrentAmmo<=0)
	{
		StartReload();
		return;
	}
	
	FHitResult HitResult;
	if (!GetAimHitResult(HitResult)) return;

	AActor* TargetActor=HitResult.GetActor();

	if (TargetActor && TargetActor!=GetOwner())
	{
		bIsTryingToLockOn = true;
		bIsLockedOn = false;
		LockOnDurationCounter=0.f;
		TrackingTarget=TargetActor;
		SetActorTickEnabled(true);
	}
}

void AHomingWeapon::StopAttack()
{
	if (!bIsTryingToLockOn) return;

	bIsTryingToLockOn = false;
	SetActorTickEnabled(false);
	OnLockOnCanceled.Broadcast();

	if (bIsLockedOn && TrackingTarget.IsValid())
	{
		FireHomingBullet();
	}
	
	bIsLockedOn = false;
	TrackingTarget=nullptr;
}

// Called when the game starts or when spawned
void AHomingWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void AHomingWeapon::FireHomingBullet()
{
	if (!TrackingTarget.IsValid()) return;

	FireMultiBullet(TrackingTarget.Get());

	--TypeSpecificInfo.AmmoInfo.CurrentAmmo;
	ApplyRecoil();
	StartAttackCooldown();
}

// Called every frame
void AHomingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsTryingToLockOn || !TrackingTarget.IsValid())
	{
		StopAttack();
		return;
	}

	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!CR4S_ENSURE(LogHong1,PC))
	{
		StopAttack();
		return;
	}

	FVector TargetLocation=TrackingTarget->GetActorLocation();
	FVector2D TargetScreenLocation;

	if (PC->ProjectWorldLocationToScreen(TargetLocation, TargetScreenLocation))
	{
		int32 ViewportX, ViewportY;
		PC->GetViewportSize(ViewportX, ViewportY);
		const float ActualPixelRadius=ViewportY*TypeSpecificInfo.HomingInfo.LockOnMaintainRadius;
		const FVector2D ScreenCenter(ViewportX*0.5f,ViewportY*0.5f);
		
		const float DistanceFromCenter=FVector2D::Distance(ScreenCenter,TargetScreenLocation);
		if (DistanceFromCenter<=ActualPixelRadius)
		{
			LockOnDurationCounter+=DeltaTime;
			OnTryingToLockOn.Broadcast(TargetScreenLocation);
			if (!bIsLockedOn && LockOnDurationCounter>=TypeSpecificInfo.HomingInfo.LockOnTime)
			{
				bIsLockedOn=true;
				OnLockOnSucceeded.Broadcast();
			}
		}
		else
		{
			StopAttack();
		}
	}
	else
	{
		StopAttack();
	}
}

