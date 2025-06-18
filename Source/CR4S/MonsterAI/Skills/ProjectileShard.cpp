#include "MonsterAI/Skills/ProjectileShard.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AProjectileShard::AProjectileShard()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
    CollisionComp->SetupAttachment(RootComp);

    CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
    CollisionComp->SetGenerateOverlapEvents(true);

    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileShard::OnOverlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bAutoActivate = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AProjectileShard::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
}

void AProjectileShard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const float Distance = FVector::DistSquared(GetActorLocation(), SpawnLocation);
    if (Distance > FMath::Square(MaxTravelDistance))
    {
        DestroyShard();
    }
}

void AProjectileShard::InitializeDirection(const FVector& InDirection, float InSpeed, float ExplosionRange)
{
    if (!ProjectileMovement) return;

    ProjectileMovement->Velocity = InDirection.GetSafeNormal() * InSpeed;
    ProjectileMovement->Activate();
    MaxTravelDistance = ExplosionRange;
}

void AProjectileShard::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void AProjectileShard::DestroyShard()
{
    Destroy();
}
