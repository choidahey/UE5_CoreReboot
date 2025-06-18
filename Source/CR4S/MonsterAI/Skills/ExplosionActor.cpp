#include "MonsterAI/Skills/ExplosionActor.h"
#include "MonsterAI/Region/AlgorVexSnowBoss.h"
#include "MonsterAI/Skills/ProjectileShard.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AExplosionActor::AExplosionActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);

	Cast<USphereComponent>(CollisionComp)->SetSphereRadius(ShockwaveRadius);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AExplosionActor::OnOverlap);
}

void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		ShockwaveRadius,
		32,
		FColor::Cyan,
		false,
		5.0f,
		0,
		2.0f
	);
#endif

	ApplyShockwaveDamage();

	if (AAlgorVexSnowBoss* Boss = Cast<AAlgorVexSnowBoss>(GetOwner()))
	{
		if (Boss->bIsIceClawBuffActive)
		{
			Boss->bIsIceClawBuffActive = false;
			SpawnShardProjectiles();
		}
	}

	if (ExplosionRadiusEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ExplosionRadiusEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(ShockwaveRadius / 100.f),
			true
		);
	}
}

void AExplosionActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AExplosionActor::ApplyShockwaveDamage()
{
	if (!IsValid(CollisionComp)) return;

	TArray<AActor*> OverlappingActors;
	CollisionComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		ApplyEffectToActor(Actor);
	}
}

void AExplosionActor::SpawnShardProjectiles()
{
	if (!IsValid(ShardProjectileClass)) return;

	const FVector Origin = GetActorLocation();

	for (int32 i = 0; i < ShardCount; ++i)
	{
		const FVector Dir = FMath::VRand();
		const FVector SpawnLoc = Origin + Dir * 100.f;
		const FRotator SpawnRot = FRotationMatrix::MakeFromZ(Dir).Rotator();

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = GetInstigator();

		AProjectileShard* Shard = GetWorld()->SpawnActor<AProjectileShard>(ShardProjectileClass, SpawnLoc, SpawnRot, Params);
		if (Shard)
		{
			Shard->InitializeDirection(Dir, LaunchSpeed, LaunchDistance);
		}
	}
}
