// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingBullet.h"

#include "CR4S.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AHomingBullet::AHomingBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AHomingBullet::Initialize(const FBulletInfo& InData, const float InDamage, AActor* HomingTarget)
{
	Super::Initialize(InData,InDamage);

	HomingTargetActor = HomingTarget;

	if (HomingTargetActor.IsValid() && BulletInfo.HomingActivationDelay>KINDA_SMALL_NUMBER)
	{
		GetWorld()->GetTimerManager().SetTimer(
			HomingActivationTimerHandle,
			this,
			&AHomingBullet::ActivateHoming,
			BulletInfo.HomingActivationDelay,
			false
		);
	}
	else
	{
		ActivateHoming();
	}
}

// Called when the game starts or when spawned
void AHomingBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHomingBullet::ActivateHoming()
{
	if (HomingTargetActor.IsValid()
		&& ProjectileMovementComponent
		&& BulletInfo.HomingStrength>KINDA_SMALL_NUMBER)
	{
		ProjectileMovementComponent->bIsHomingProjectile=true;
		ProjectileMovementComponent->HomingTargetComponent=HomingTargetActor->GetRootComponent();
		ProjectileMovementComponent->HomingAccelerationMagnitude=BulletInfo.HomingStrength;
	}
}

// Called every frame
void AHomingBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

