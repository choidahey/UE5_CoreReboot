// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBullet.h"

#include "CR4S.h"
#include "NiagaraComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Data/WeaponData.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseBullet::ABaseBullet()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneComponent=CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(SceneComponent);
	
	CollisionComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel2);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2,ECR_Ignore);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetupAttachment(RootComponent);

	NiagaraComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComponent->SetupAttachment(CollisionComponent);

	ProjectileMovementComponent=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->bRotationFollowsVelocity=false;
	ProjectileMovementComponent->bShouldBounce=false;
	ProjectileMovementComponent->UpdatedComponent=RootComponent;
	
	ProjectileMovementComponent->InitialSpeed=0.f;
	ProjectileMovementComponent->MaxSpeed=0.f;
	ProjectileMovementComponent->ProjectileGravityScale=0.f;
	
	ImpactParticle=nullptr;
	ImpactSound=nullptr;
	
}

void ABaseBullet::Initialize(const FBulletData& InData)
{
	// ImpactParticle=InData.ImpactParticle;
	// ImpactSound=InData.ImpactSound;
	ProjectileMovementComponent->InitialSpeed=InData.InitialBulletSpeed;
	ProjectileMovementComponent->MaxSpeed=InData.MaxBulletSpeed;
	ExplosionRadius=InData.ExplosionRadius;
	//Homing

	if (CollisionComponent)
	{
		if (AActor* OwnerActor=GetOwner())
		{
			CollisionComponent->IgnoreActorWhenMoving(OwnerActor,true);
		}
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this,&ABaseBullet::OnOverlapBegin);
	}
}

// Called when the game starts or when spawned
void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();
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
	FVector OverlapLocation=GetActorLocation();
	AActor* OwnerActor=GetOwner();
	if (ExplosionRadius<=KINDA_SMALL_NUMBER) // 
	{
		if (OtherActor&&OtherActor!=this)
		{
			UGameplayStatics::ApplyDamage(
				OtherActor,
				Damage,
				OwnerActor ? OwnerActor->GetInstigatorController():nullptr,
				Owner,
				UDamageType::StaticClass()
			);
		}
	}
	else // Explosion Bullet
	{
		TArray<AActor*> IgnoreList;
		IgnoreList.Add(this);

		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			Damage,
			SweepResult.ImpactPoint,
			ExplosionRadius,
			UDamageType::StaticClass(),
			IgnoreList,
			Owner,
			OwnerActor ? OwnerActor->GetInstigatorController():nullptr,
			true
		);
	}
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
	
	Destroy();
}

// Called every frame
void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

