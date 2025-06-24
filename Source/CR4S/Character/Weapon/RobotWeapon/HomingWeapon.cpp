// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingWeapon.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "GameFramework/HUD.h"
#include "UI/InGame/SurvivalHUD.h"


// Sets default values
AHomingWeapon::AHomingWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AHomingWeapon::OnAttack()
{
	CR4S_SIMPLE_SCOPE_LOG;
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
		bIsAttackButtonHeldDown=true;
		
		bIsTryingToLockOn = true;
		bIsLockedOn = false;
		LockOnDurationCounter=0.f;
		TrackingTarget=TargetActor;
		SetActorTickEnabled(true);
		OnLockOnStarted.Broadcast();
	}
	Super::OnAttack();
}

void AHomingWeapon::Initialize(AModularRobot* OwnerCharacter, const int32 SlotIdx)
{
	Super::Initialize(OwnerCharacter, SlotIdx);
}

void AHomingWeapon::StopAttack()
{
	CR4S_SIMPLE_SCOPE_LOG;
	if (!bIsAttackButtonHeldDown) return;

	bIsAttackButtonHeldDown=false;
	
	bIsTryingToLockOn = false;
	SetActorTickEnabled(false);
	OnLockOnCanceled.Broadcast();

	if (bIsLockedOn && TrackingTarget.IsValid())
	{
		FireHomingBullet();
	}
	
	bIsLockedOn = false;
	TrackingTarget=nullptr;
	Super::StopAttack();
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

	AddCurrentAmmo(-1);
	ApplyRecoil();
	StartAttackCooldown();
}

// Called every frame
void AHomingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsTryingToLockOn)
	{
		SetActorTickEnabled(false);
		return;
	}

	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());

	bool bShouldCancelLockOn=false;
	if (!CR4S_ENSURE(LogHong1,PC && TrackingTarget.IsValid()))
	{
		bShouldCancelLockOn=true;
	}
	else
	{
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
					OnLockOnFinished.Broadcast();
				}
			}
			else
			{
				bIsLockedOn=false;
				bShouldCancelLockOn=true;	
			}
		}
		else
		{
			bIsLockedOn=false;
			bShouldCancelLockOn=true;
		}
	}

	if (bShouldCancelLockOn)
	{
		OnLockOnCanceled.Broadcast();
		bIsTryingToLockOn=false;
		bIsLockedOn=false;
		TrackingTarget=nullptr;
		SetActorTickEnabled(false);
	}
}

