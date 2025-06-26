#include "MonsterAI/Skills/ProjectileSlashActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"

AProjectileSlashActor::AProjectileSlashActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Override the base CollisionComp to use BoxComponent
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionProfileName("MonsterSkillActor");
	CollisionComp->SetGenerateOverlapEvents(true);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileSlashActor::OnOverlap);

	// Trail effect attached to StaticMesh
	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(StaticMesh);
	TrailEffect->SetAutoActivate(false);

	// Destroy effect (spawned manually)
	DestroyEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DestroyEffect"));
	DestroyEffect->SetupAttachment(RootComp);
	DestroyEffect->SetAutoActivate(false);
}

void AProjectileSlashActor::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	TrailEffect->Activate();
}

void AProjectileSlashActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Delta = MoveDirection * MoveSpeed * DeltaTime;
	AddActorWorldOffset(Delta, true);

	if (FVector::DistSquared(StartLocation, GetActorLocation()) > FMath::Square(MaxTravelDistance))
	{
		if (DestroyEffect && !DestroyEffect->IsActive())
		{
			DestroyEffect->Activate(true);
		}

		Destroy();
	}
}

void AProjectileSlashActor::InitializeMovement(float InSpeed, float InDistance, const FVector& InStartLocation)
{
	MoveDirection = GetActorForwardVector();
	MoveDirection.Z = 0.f;
	MoveDirection.Normalize();

	MoveSpeed = InSpeed;
	MaxTravelDistance = InDistance;
	StartLocation = InStartLocation;
}

void AProjectileSlashActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
