// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeapon.h"
#include "BaseBullet.h"
#include "Character/Characters/ModularRobot.h"
#include "GameFramework/ProjectileMovementComponent.h"


URangedWeapon::URangedWeapon()
{
	Range=10000.0f;
}



void URangedWeapon::OnAttack(const int32 WeaponIdx)
{
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
	
	FVector TraceEnd=WorldOrigin+(WorldDirection*Range);
	
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

	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator=OwningCharacter;
		SpawnParameters.Owner = OwningCharacter;

		FRotator SpawnRotation=ShootDirection.Rotation();

		ABaseBullet* NewProjectile=GetWorld()->SpawnActor<ABaseBullet>(
			ProjectileClass,
			MuzzleLocation,
			SpawnRotation,
			SpawnParameters
		);
	}
}