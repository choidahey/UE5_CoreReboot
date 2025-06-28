// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeapon.h"
#include "CR4S.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RewindData.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/Bullet/BaseBullet.h"
#include "FriendlyAI/Component/ObjectPoolComponent.h"
#include "Game/System/AudioManager.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "Utility/DataLoaderSubsystem.h"

ARangedWeapon::ARangedWeapon()
{
}

void ARangedWeapon::Initialize(AModularRobot* OwnerCharacter, const int32 SlotIdx)
{
	UGameInstance* GI=GetGameInstance();
	if (!GI) return;

	UDataLoaderSubsystem* DataLoader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!DataLoader||!ToolTag.IsValid()) return;

	const bool bSuccess=DataLoader->LoadWeaponInfoByTag(ToolTag,TypeSpecificInfo,BaseInfo);
	if (!CR4S_ENSURE(LogHong1,bSuccess))
	{
		return;
	}
	Super::Initialize(OwnerCharacter, SlotIdx);
}

int32 ARangedWeapon::GetCurrentAmmo() const
{
	return TypeSpecificInfo.AmmoInfo.CurrentAmmo;
}

float ARangedWeapon::GetCurrentAmmoPercentage() const
{
	if (TypeSpecificInfo.AmmoInfo.MagazineCapacity<=KINDA_SMALL_NUMBER)
	{
		return 0;
	}
	const int32 MaxAmmo=TypeSpecificInfo.AmmoInfo.MagazineCapacity;
	const int32 CurrentAmmo=TypeSpecificInfo.AmmoInfo.CurrentAmmo;
	const float Percent=FMath::Clamp(CurrentAmmo/MaxAmmo,0.f,1.f);
	return Percent;
}

void ARangedWeapon::StartSequentialFire(AActor* HomingTarget)
{
	GetWorld()->GetTimerManager().ClearTimer(SequentialFireTimerHandle);

	CurrentHomingTarget=HomingTarget;
	const bool bIsMultiShot=!TypeSpecificInfo.MultiShotInfo.MuzzleSocketNames.IsEmpty();
	
	if (bIsMultiShot)
	{
		ShotsRemainingInSequence=TypeSpecificInfo.MultiShotInfo.MuzzleSocketNames.Num();
		MuzzleIndexInSequence=0;
	}
	else
	{
		ShotsRemainingInSequence=TypeSpecificInfo.BurstShotInfo.ShotsPerBurst;
		MuzzleIndexInSequence=0;
	}
	

	if (ShotsRemainingInSequence<=0) return;

	GetWorld()->GetTimerManager().SetTimer(
		SequentialFireTimerHandle,
		this,
		&ARangedWeapon::FireNextShotInSequence,
		TypeSpecificInfo.BurstShotInfo.TimeBetweenShots,
		true
	);
}

void ARangedWeapon::FireNextShotInSequence()
{
	if (ShotsRemainingInSequence<=0 || GetCurrentAmmo()<=0)
	{
		GetWorld()->GetTimerManager().ClearTimer(SequentialFireTimerHandle);
		if (GetCurrentAmmo()<=0)
		{
			StartReload();
		}
		return;
	}

	const bool bIsMultiShot=!TypeSpecificInfo.MultiShotInfo.MuzzleSocketNames.IsEmpty();
	FName MuzzleName;
	
	if (bIsMultiShot)
	{
		MuzzleName=TypeSpecificInfo.MultiShotInfo.MuzzleSocketNames[MuzzleIndexInSequence++];
	}
	else
	{
		MuzzleName=TypeSpecificInfo.MuzzleSocketName;
	}

	FHitResult HitResult;
	if (!GetAimHitResult(HitResult)) return;

	const FVector MuzzleLocation=GetMuzzleLocation(MuzzleName);
	const FVector ShootDirection = (HitResult.ImpactPoint-MuzzleLocation).GetSafeNormal();

	if (!ShootDirection.IsNearlyZero() && TypeSpecificInfo.ProjectileClass)
	{
		PlayMuzzleVFX(MuzzleName);
		PlayMuzzleSFX(MuzzleName);
		
		const FRotator SpawnRotation=ShootDirection.Rotation();
		FireBullet(MuzzleLocation, SpawnRotation, CurrentHomingTarget.Get());
		AddCurrentAmmo(-1);
		ApplyRecoil();
	}

	--ShotsRemainingInSequence;
}

void ARangedWeapon::RefreshUI()
{
	AddCurrentAmmo(0);
}

void ARangedWeapon::FireMultiBullet(AActor* HomingTarget)
{
	const TArray<FName> Muzzles=TypeSpecificInfo.MultiShotInfo.MuzzleSocketNames;
	
	if (!CR4S_ENSURE(LogHong1,!Muzzles.IsEmpty()
		&& TypeSpecificInfo.ProjectileClass))
	{
		return;
	}

	FHitResult HitResult;
	if (!GetAimHitResult(HitResult)) return;

	for (const FName& MuzzleName:Muzzles)
	{
		const FVector MuzzleLocation=GetMuzzleLocation(MuzzleName);

		const FVector TargetLocation = HitResult.ImpactPoint;
		const FVector ShootDirection=(TargetLocation-MuzzleLocation).GetSafeNormal();

		if (ShootDirection.IsNearlyZero()) continue;

		const FRotator SpawnRotation=ShootDirection.Rotation();

		FireBullet(MuzzleLocation, SpawnRotation, HomingTarget);
	}
}

bool ARangedWeapon::GetAimHitResult(FHitResult& OutHitResult) const
{
	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!PC) return false;

	int32 ViewportX, ViewportY;
	PC->GetViewportSize(ViewportX, ViewportY);
	float ScreenX=ViewportX*0.5f;
	float ScreenY=ViewportY*0.5f;

	FVector WorldOrigin;
	FVector WorldDirection;
	const bool bDeprojected=PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldOrigin, WorldDirection);
	if (!bDeprojected) return false;
	
	const FVector TraceEnd=WorldOrigin+(WorldDirection*TypeSpecificInfo.MaxAimTrackingRange);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwningCharacter);

	const bool bHit=GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WorldOrigin,
		TraceEnd,
		ECC_GameTraceChannel5,
		Params
	);
	if (!bHit)
	{
		OutHitResult.ImpactPoint=TraceEnd;
	}

	return true;
}

FVector ARangedWeapon::GetMuzzleLocation(const FName& SocketName) const
{
	if (!OwningCharacter||!OwningCharacter->GetMesh()) return FVector::ZeroVector;
	
	return StaticMeshComp->GetSocketLocation(SocketName);
}

void ARangedWeapon::FireBullet(const FVector& MuzzleLocation, const FRotator& SpawnRotation, AActor* HomingTarget)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	UProjectilePoolSubsystem* PoolSubsystem = World->GetSubsystem<UProjectilePoolSubsystem>();
	if (!PoolSubsystem) return;
	
	AActor* NewProjectile = PoolSubsystem->SpawnFromPool(
		TypeSpecificInfo.ProjectileClass,
		MuzzleLocation,
		SpawnRotation
	);

	ABaseBullet* Projectile = Cast<ABaseBullet>(NewProjectile);
	if (!CR4S_ENSURE(LogHong1,Projectile)) return;
	
	Projectile->SetInstigator(OwningCharacter);
	Projectile->SetOwner(OwningCharacter);
	const float FinalDamage=ComputeFinalDamage();
	Projectile->Initialize(TypeSpecificInfo.BulletInfo,FinalDamage,HomingTarget);
}

void ARangedWeapon::ApplyRecoil() const
{
	if (TypeSpecificInfo.RecoilInfo.Recoil<=KINDA_SMALL_NUMBER || !OwningCharacter) return;

	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!PC) return;

	const float RecoilModifier=OwningCharacter->GetRecoilModifier();

	const float FinalRecoil=TypeSpecificInfo.RecoilInfo.Recoil*RecoilModifier;
	const float HorizontalMultiplier=TypeSpecificInfo.RecoilInfo.HorizontalRecoilMultiplier; 
	const float HorizontalRecoil=FMath::FRandRange(-FinalRecoil*HorizontalMultiplier,FinalRecoil*HorizontalMultiplier);
	PC->AddPitchInput(FinalRecoil);
	PC->AddYawInput(HorizontalRecoil);
}

void ARangedWeapon::StartReload()
{
	if (bIsReloading || TypeSpecificInfo.AmmoInfo.CurrentAmmo==TypeSpecificInfo.AmmoInfo.MagazineCapacity) return;

	bIsReloading=true;
	bCanAttack=false;

	OnStartReload.Broadcast(TypeSpecificInfo.AmmoInfo.ReloadTime);
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&ARangedWeapon::FinishReload,
		TypeSpecificInfo.AmmoInfo.ReloadTime,
		false
	);
}

void ARangedWeapon::FinishReload()
{
	SetCurrentAmmo(TypeSpecificInfo.AmmoInfo.MagazineCapacity);
	bIsReloading=false;
	bCanAttack=true;
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void ARangedWeapon::AddCurrentAmmo(const int32 Amount)
{
	SetCurrentAmmo(TypeSpecificInfo.AmmoInfo.CurrentAmmo+Amount);
}

void ARangedWeapon::SetCurrentAmmo(const int32 NewAmount)
{
	const int32 MaxAmmo=TypeSpecificInfo.AmmoInfo.MagazineCapacity;
	const int32 NewCurrentAmmo=FMath::Clamp(NewAmount,0,MaxAmmo);
	TypeSpecificInfo.AmmoInfo.CurrentAmmo=NewCurrentAmmo;

	const float Percent = (MaxAmmo>0) ? static_cast<float>(NewCurrentAmmo)/MaxAmmo : 0;
	OnCurrentAmmoChanged.Broadcast(Percent);
}

void ARangedWeapon::PlayMuzzleVFX(const FName& MuzzleSocketName) const
{
	if (!CR4S_ENSURE(LogHong1,TypeSpecificInfo.MuzzleParticle)) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		TypeSpecificInfo.MuzzleParticle,
		StaticMeshComp,
		MuzzleSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);
}

void ARangedWeapon::PlayMuzzleSFX(const FName& MuzzleSocketName) const
{
	if (!CR4S_ENSURE(LogHong1,TypeSpecificInfo.MuzzleSound)) return;

	const FVector MuzzleLocation=GetMuzzleLocation(MuzzleSocketName);
	if (TypeSpecificInfo.MuzzleSound)
	{
		if (UGameInstance* GI=GetGameInstance())
		{
			if (UAudioManager* Audio=GI->GetSubsystem<UAudioManager>())
			{
				Audio->PlaySFX(
					TypeSpecificInfo.MuzzleSound,
					MuzzleLocation,
					EConcurrencyType::Impact
				);
			}
		}
	}
}

void ARangedWeapon::StartMuzzleFlash(const FName& MuzzleSocketName)
{
	if (!CR4S_ENSURE(LogHong1,TypeSpecificInfo.MuzzleParticle)) return;

	if (MuzzleFlashComp && MuzzleFlashComp->IsActive()) return;

	MuzzleFlashComp=UNiagaraFunctionLibrary::SpawnSystemAttached(
		TypeSpecificInfo.MuzzleParticle,
		StaticMeshComp,
		MuzzleSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);
}

void ARangedWeapon::StopMuzzleFlash()
{
	if (!MuzzleFlashComp) return;

	MuzzleFlashComp->Deactivate();

	MuzzleFlashComp->DestroyComponent();
	MuzzleFlashComp=nullptr;
}

