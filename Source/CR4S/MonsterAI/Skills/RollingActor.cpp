#include "MonsterAI/Skills/RollingActor.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/SphereComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

ARollingActor::ARollingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RollingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RollingMesh"));
	SetRootComponent(RollingMesh);

	RollingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RollingMesh->SetCollisionObjectType(ECC_PhysicsBody);
	RollingMesh->SetCollisionResponseToAllChannels(ECR_Block);
	RollingMesh->SetSimulatePhysics(true);
	RollingMesh->SetEnableGravity(true);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComp->SetupAttachment(RollingMesh);

	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ARollingActor::OnOverlap);
}

void ARollingActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorScale3D(ScaleFactor);
	StartLocation = GetActorLocation();

	RollingMesh->SetEnableGravity(true);
	RollingMesh->SetLinearDamping(RollingLinearDamping);
	RollingMesh->SetAngularDamping(RollingAngularDamping);
	RollingMesh->SetMassOverrideInKg(NAME_None, RollingMassInKg);

	RollingMesh->BodyInstance.bOverrideMaxAngularVelocity = true;
	RollingMesh->BodyInstance.MaxAngularVelocity = MaxRollingAngularVelocity;

	GetWorld()->GetTimerManager().SetTimer(
		BreakTimerHandle,
		this,
		&ARollingActor::BreakAndDestroy,
		MaxLifetime,
		false
	);
}

void ARollingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastKnownVelocity = RollingMesh->GetPhysicsLinearVelocity();

	const float Distance = FVector::DistSquared(StartLocation, GetActorLocation());
	if (Distance >= FMath::Square(MaxDistance))
	{
		BreakAndDestroy();
	}
}

void ARollingActor::LaunchInDirection(const FVector& Direction)
{
	if (!CollisionComp) return;

	FVector LaunchDir = Direction.GetSafeNormal();
	LaunchDir.Z = -0.2f;
	LaunchDir = LaunchDir.GetSafeNormal();

	const FVector Velocity = LaunchDir * LaunchSpeed;
	RollingMesh->SetPhysicsLinearVelocity(Velocity);
}

void ARollingActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,	const FHitResult& SweepResult)
{
	if (bHasBroken) return;
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

void ARollingActor::BreakAndDestroy()
{
	if (!IsValid(this) || bHasBroken) return;
	bHasBroken = true;

	GetWorld()->GetTimerManager().ClearTimer(BreakTimerHandle);

	if (!BreakGeometryClass)
	{
		Destroy();
		return;
	}

#if WITH_EDITOR
	if (GIsEditor && !GetWorld()->IsPlayInEditor())
	{
		// Prevent crash in Animation Preview or Fracture Editor
		UE_LOG(LogTemp, Warning, TEXT("[RollingActor] Skipped BreakAndDestroy during editor preview."));
		Destroy();
		return;
	}
#endif

	const FVector AngularVel = RollingMesh->GetPhysicsAngularVelocityInDegrees();
	const FQuat Rotation = GetActorQuat();

	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.SetScale3D(ScaleFactor);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGeometryCollectionActor* BreakActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(
		BreakGeometryClass, SpawnTransform, Params);

	if (BreakActor && BreakActor->GetGeometryCollectionComponent())
	{
		UGeometryCollectionComponent* GC = BreakActor->GetGeometryCollectionComponent();

		if (GC->RestCollection)
		{
			GC->SetWorldScale3D(ScaleFactor);
			GC->SetAllPhysicsLinearVelocity(LastKnownVelocity);
			GC->SetPhysicsAngularVelocityInDegrees(AngularVel);

			const FVector ImpulseDir = LastKnownVelocity.GetSafeNormal();
			const FVector Impulse = ImpulseDir * ImpulseStrength;

			GC->SetSimulatePhysics(true);
			GC->AddImpulse(Impulse, NAME_None, true);
		}

		BreakActor->SetActorRotation(Rotation);
		BreakActor->SetLifeSpan(5.0f);
	}

	Destroy();
}

void ARollingActor::Destroyed()
{
	Super::Destroyed();
}
