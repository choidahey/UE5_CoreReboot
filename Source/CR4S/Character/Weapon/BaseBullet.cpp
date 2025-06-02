// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBullet.h"
#include "CR4S.h"
#include "NiagaraComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ABaseBullet::ABaseBullet()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent=CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(SceneComponent);
	
	CollisionComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetupAttachment(RootComponent);

	NiagaraComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComponent->SetupAttachment(CollisionComponent);

	RadialForceComponent=CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(CollisionComponent);
	RadialForceComponent->Radius=300.f;
	RadialForceComponent->ImpulseStrength=200.f;
	RadialForceComponent->bAutoActivate=false;
	RadialForceComponent->bIgnoreOwningActor=true;

	ProjectileMovementComponent=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed=2000.f;
	ProjectileMovementComponent->MaxSpeed=3000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity=true;
	ProjectileMovementComponent->bShouldBounce=false;
	ProjectileMovementComponent->UpdatedComponent=RootComponent;
	
	ImpactParticle=nullptr;
	ImpactSound=nullptr;

	BaseDamage=100.f;
}

// Called when the game starts or when spawned
void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ABaseBullet::OnOverlapBegin);
		if (AActor* OwnerActor=GetOwner())
		{
			CollisionComponent->IgnoreActorWhenMoving(OwnerActor,true);
		}
	}
}

void ABaseBullet::OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	)
{
	if (RadialForceComponent)
	{
		RadialForceComponent->FireImpulse();
	}

	FVector OverlapLocation=GetActorLocation();
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticle,
			OverlapLocation,
			FRotator::ZeroRotator,
			true
		);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			OverlapLocation,
			1.0f,
			1.0f
		);
	}

	if (OtherActor&&OtherActor!=this)
	{
		AActor* OwnerActor=GetOwner();
		float FinalDamage=BaseDamage;
		if (OwnerActor)
		{
			if (AModularRobot* OwnerRobot=Cast<AModularRobot>(OwnerActor))
			{
				float AttackMultiplier=OwnerRobot->GetAttackPowerMultiplier();
				FinalDamage*=AttackMultiplier;
			}
		}
		UGameplayStatics::ApplyDamage(
			OtherActor,
			FinalDamage,
			OwnerActor ? OwnerActor->GetInstigatorController():nullptr,
			this,
			UDamageType::StaticClass()
		);
	}
	
	Destroy();
}

// Called every frame
void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

