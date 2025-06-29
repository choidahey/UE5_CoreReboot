// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBullet.h"

#include "CR4S.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Data/WeaponData.h"
#include "Components/BoxComponent.h"
#include "FriendlyAI/Component/ObjectPoolComponent.h"
#include "Game/System/AudioManager.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/CombatStatics.h"

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

	PoolComponent=CreateDefaultSubobject<UObjectPoolComponent>(TEXT("PoolComponent"));
	
}

void ABaseBullet::Initialize(const FBulletInfo& InData, const float InDamage, AActor* HomingTarget)
{
	BulletInfo=InData;
	Damage=InDamage;

	if (!ProjectileMovementComponent) return;

	ProjectileMovementComponent->Activate();
	ProjectileMovementComponent->InitialSpeed=BulletInfo.InitialBulletSpeed;
	ProjectileMovementComponent->MaxSpeed=BulletInfo.MaxBulletSpeed;
	ProjectileMovementComponent->Velocity=GetActorForwardVector()*BulletInfo.InitialBulletSpeed;
	
	if (BulletInfo.MaxLifeTime>KINDA_SMALL_NUMBER)
	{
		PoolComponent->ReturnToPoolAfter(BulletInfo.MaxLifeTime);
	}
	else
	{
		if (UWorld* CurrentWorld = GetWorld())
		{
			if (UProjectilePoolSubsystem* Pool = CurrentWorld->GetSubsystem<UProjectilePoolSubsystem>())
			{
				Pool->ReturnToPool(this);
			}
		}
	}

	if (CollisionComponent)
	{
		AActor* OwnerActor=GetOwner();
		if (CR4S_ENSURE(LogHong1,OwnerActor))
		{
			CollisionComponent->IgnoreActorWhenMoving(OwnerActor,true);
		}
		CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this,&ABaseBullet::OnOverlapBegin);
	}
}

void ABaseBullet::Deactivate()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.RemoveAll(this);
	}
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->Velocity=FVector::ZeroVector;
	}
}

// Called when the game starts or when spawned
void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();
	PoolComponent->OnReturnToPoolDelegate.AddUniqueDynamic(this,&ABaseBullet::Deactivate);
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

	if (OwnerActor==OtherActor) return;
	
	if (BulletInfo.ExplosionRadius<=KINDA_SMALL_NUMBER) 
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
			BulletInfo.ExplosionRadius,
			UDamageType::StaticClass(),
			IgnoreList,
			Owner,
			OwnerActor ? OwnerActor->GetInstigatorController():nullptr,
			true
		);
	}
	if (BulletInfo.ImpactParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BulletInfo.ImpactParticle,
			OverlapLocation,
			FRotator::ZeroRotator
		);
	}
	if (BulletInfo.ImpactSound)
	{
		if (BulletInfo.ImpactSound)
		{
			if (UGameInstance* GI=GetGameInstance())
			{
				if (UAudioManager* Audio=GI->GetSubsystem<UAudioManager>())
				{
					Audio->PlaySFX(
						BulletInfo.ImpactSound,
						OverlapLocation,
						EConcurrencyType::Impact
					);
				}
			}
		}
	}
	UCombatStatics::ApplyStun(OtherActor,BulletInfo.StunAmount);

	if (UWorld* CurrentWorld = GetWorld())
	{
		if (UProjectilePoolSubsystem* Pool = CurrentWorld->GetSubsystem<UProjectilePoolSubsystem>())
		{
			Pool->ReturnToPool(this);
		}
	}
}

// Called every frame
void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

