// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeapon.h"
#include "BaseBullet.h"
#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Utility/DataLoaderSubsystem.h"

URangedWeapon::URangedWeapon()
{
}

void URangedWeapon::OnAttack(const int32 WeaponIdx)
{
	if (!bCanAttack) return;
	
	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!PC) return;

	int32 ViewportX, ViewportY;
	PC->GetViewportSize(ViewportX, ViewportY);
	float ScreenX=ViewportX*0.5f;
	float ScreenY=ViewportY*0.5f;

	FVector WorldOrigin;
	FVector WorldDirection;
	bool bDeprojected=PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldOrigin, WorldDirection);
	if (!bDeprojected) return;
	
	FVector TraceEnd=WorldOrigin+(WorldDirection*TypeSpecificInfo.Range);
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwningCharacter);

	bool bHit=GetWorld()->LineTraceSingleByChannel(
		Hit,
		WorldOrigin,
		TraceEnd,
		ECC_Visibility,
		Params
	);

	FVector ImpactPoint;
	if (bHit)
	{
		ImpactPoint=Hit.ImpactPoint;
	}
	else
	{
		ImpactPoint=TraceEnd;
	}
	FString SocketNameString = FString::Printf(TEXT("Muzzle_%d"), WeaponIdx);  // "Muzzle_2"
	FName   SocketName(*SocketNameString); 
	FVector MuzzleLocation=OwningCharacter->GetMesh()->GetSocketLocation(SocketName);

	FVector ShootDirection=(ImpactPoint-MuzzleLocation).GetSafeNormal();

	if (!CR4S_ENSURE(LogHong1, TypeSpecificInfo.ProjectileClass))
	{
		return;
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator=OwningCharacter;
	SpawnParameters.Owner = OwningCharacter;

	FRotator SpawnRotation=ShootDirection.Rotation();

	ABaseBullet* NewProjectile=GetWorld()->SpawnActor<ABaseBullet>(
		TypeSpecificInfo.ProjectileClass,
		MuzzleLocation,
		SpawnRotation,
		SpawnParameters
	);
	float FinalDamage=ComputeFinalDamage();
	NewProjectile->Initialize(TypeSpecificInfo.BulletInfo,FinalDamage);
	
	StartAttackCooldown();
}

void URangedWeapon::Initialize(AModularRobot* OwnerCharacter)
{
	Super::Initialize(OwnerCharacter);

	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!GI) return;

	UDataLoaderSubsystem* DataLoader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!DataLoader||!WeaponTag.IsValid()) return;

	const bool bSuccess=DataLoader->LoadWeaponInfoByTag(WeaponTag,TypeSpecificInfo,BaseInfo);
	if (!CR4S_ENSURE(LogHong1,bSuccess))
	{
		return;
	}
}
