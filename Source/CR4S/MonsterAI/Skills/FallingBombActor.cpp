#include "MonsterAI/Skills/FallingBombActor.h"
#include "MonsterAI/BaseMonster.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Skills/ProjectileShard.h"

AFallingBombActor::AFallingBombActor()
{
	PrimaryActorTick.bCanEverTick = true;

	if (StaticMesh)
	{
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetSimulatePhysics(false);
		StaticMesh->SetEnableGravity(false);
	}
	
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);

	LandingTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LandingTrigger"));
	LandingTrigger->SetupAttachment(RootComp);
	LandingTrigger->SetGenerateOverlapEvents(true);
}

void AFallingBombActor::Initialize(AActor* InOwner, AActor* InTarget)
{
	const FVector Center = InTarget ? InTarget->GetActorLocation() : GetActorLocation();
	const FVector2D Offset = FMath::RandPointInCircle(RandomSpawnRadius);
	const float RandomZ = FMath::FRandRange(MinSpawnHeight, MaxSpawnHeight);
	const FVector SpawnLoc = Center + FVector(Offset.X, Offset.Y, RandomZ);

	SetActorLocation(SpawnLoc);
	CurrentFallSpeed = 0.f;
	bHasLanded = false;
	bHasExploded = false;

	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AFallingBombActor::OnOverlap);
	LandingTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &AFallingBombActor::OnLandingDetected);
}

void AFallingBombActor::BeginPlay()
{
	Super::BeginPlay();

}

void AFallingBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasLanded || bHasExploded) return;

	CurrentFallSpeed += GravityAcceleration * DeltaTime;
	FVector NewLoc = GetActorLocation();
	NewLoc.Z += CurrentFallSpeed * DeltaTime;
	SetActorLocation(NewLoc);
}

void AFallingBombActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this || bHasExploded) return;
	if (Cast<ABaseMonster>(OtherActor)) return;
	if (OtherActor->IsA(ABaseSkillActor::StaticClass())) return;

	ApplyEffectToActor(OtherActor);
}

void AFallingBombActor::OnLandingDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this || bHasExploded || bHasLanded) return;
	if (Cast<ABaseMonster>(OtherActor)) return;
	if (OtherActor->IsA(ABaseSkillActor::StaticClass())) return;

	const ECollisionChannel HitChannel = OtherComp->GetCollisionObjectType();
	if (OtherActor->IsA<ACharacter>() || HitChannel == ECC_WorldDynamic)
	{
		ShatterOnly();
	}

	if (HitChannel == ECC_WorldStatic)
	{
		bHasLanded = true;
		GetWorld()->GetTimerManager().SetTimer(
			ExplosionTimerHandle,
			this,
			&AFallingBombActor::TriggerExplosion,
			ExplosionDelay,
			false
		);
	}
}

void AFallingBombActor::TriggerExplosion()
{
	if (bHasExploded) return;
	bHasExploded = true;

	SpawnExplosionEffect();
	SpawnDirectionalBursts();
	Destroy();
	SpawnExplosionEffect();
}

void AFallingBombActor::SpawnExplosionEffect()
{
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation());
	}
}

void AFallingBombActor::SpawnDirectionalBursts()
{
	const FVector Origin = GetActorLocation();
	int32 Spawned = 0;

	for (int32 i = 0; i < MaxBurstCount * 2 && Spawned < MaxBurstCount; ++i)
	{
		const FVector Dir = FMath::VRandCone(FVector::UpVector, FMath::DegreesToRadians(ExplosionAngle));
		const FVector TraceEnd = Origin + Dir * TraceCheckLength;

		if (ShardClass)
		{
			const FVector SpawnLoc = Origin + Dir * 100.f;
			const FRotator SpawnRot = Dir.Rotation();

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AProjectileShard* Shard = GetWorld()->SpawnActor<AProjectileShard>(ShardClass, SpawnLoc, SpawnRot, Params);
			if (Shard)
			{
				const FVector Forward = Shard->GetActorUpVector();
				Shard->InitializeDirection(Forward, LaunchSpeed, ExplosionRange);
				++Spawned;
			}
		}
	}
}

void AFallingBombActor::ShatterOnly()
{
	if (ShatterEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ShatterEffect, GetActorLocation());
	}

	Destroy();
}
