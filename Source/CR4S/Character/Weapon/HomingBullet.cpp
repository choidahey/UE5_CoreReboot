// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingBullet.h"

#include "CR4S.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AHomingBullet::AHomingBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AHomingBullet::Initialize(const FBulletInfo& InData, const float InDamage, AActor* HomingTarget)
{
	Super::Initialize(InData,InDamage);
	HomingTargetActor=HomingTarget;
}

// Called when the game starts or when spawned
void AHomingBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHomingBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HomingTargetActor.IsValid() && BulletInfo.HomingStrength>0.f)
	{
		if (!CR4S_ENSURE(LogHong1,ProjectileMovementComponent)) return;

		const FVector DirectionToTarget=(HomingTargetActor->GetActorLocation()-GetActorLocation()).GetSafeNormal();

		const FVector CurrentVelocity=ProjectileMovementComponent->Velocity;
		const float CurrentSpeed=CurrentVelocity.Size();

		const FVector InterpedVelocity=FMath::VInterpTo(
			CurrentVelocity,
			DirectionToTarget*CurrentSpeed,
			DeltaTime,
			BulletInfo.HomingStrength
		);
		ProjectileMovementComponent->Velocity=InterpedVelocity;
	}
}

