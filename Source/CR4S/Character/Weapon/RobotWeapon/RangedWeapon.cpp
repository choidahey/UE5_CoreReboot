// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeapon.h"
#include "CR4S.h"
#include "RewindData.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/Bullet/BaseBullet.h"
#include "Utility/DataLoaderSubsystem.h"

ARangedWeapon::ARangedWeapon()
{
}

void ARangedWeapon::Initialize(AModularRobot* OwnerCharacter)
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
	Super::Initialize(OwnerCharacter);
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
	
	const FVector TraceEnd=WorldOrigin+(WorldDirection*TypeSpecificInfo.Range);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwningCharacter);

	const bool bHit=GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WorldOrigin,
		TraceEnd,
		ECC_Camera,
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
	
	return SkeletalMeshComp->GetSocketLocation(SocketName);
}

void ARangedWeapon::FireBullet(const FVector& MuzzleLocation, const FRotator& SpawnRotation, AActor* HomingTarget)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator=OwningCharacter;
	SpawnParams.Owner=OwningCharacter;

	ABaseBullet* NewProjectile=GetWorld()->SpawnActor<ABaseBullet>(
		TypeSpecificInfo.ProjectileClass,
		MuzzleLocation,
		SpawnRotation,
		SpawnParams
	);

	if (NewProjectile)
	{
		const float FinalDamage=ComputeFinalDamage();
		NewProjectile->Initialize(TypeSpecificInfo.BulletInfo,FinalDamage,HomingTarget);
	}
}

void ARangedWeapon::ApplyRecoil() const
{
	if (TypeSpecificInfo.RecoilInfo.Recoil<=KINDA_SMALL_NUMBER || !OwningCharacter) return;

	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!PC) return;

	float Recoil=TypeSpecificInfo.RecoilInfo.Recoil;
	float HorizontalMultiplier=TypeSpecificInfo.RecoilInfo.HorizontalRecoilMultiplier; 
	const float HorizontalRecoil=FMath::FRandRange(-Recoil*HorizontalMultiplier,Recoil*HorizontalMultiplier);
	PC->AddPitchInput(Recoil);
	PC->AddYawInput(HorizontalRecoil);
}

void ARangedWeapon::StartReload()
{
	if (bIsReloading || TypeSpecificInfo.AmmoInfo.CurrentAmmo==TypeSpecificInfo.AmmoInfo.MagazineCapacity) return;

	bIsReloading=true;
	bCanAttack=false;

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

