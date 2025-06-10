#include "MonsterAI/Skills/RollingActor.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/SphereComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

ARollingActor::ARollingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMesh->SetSimulatePhysics(true);

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComp->SetupAttachment(StaticMesh);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARollingActor::OnRollingOverlap);
}

void ARollingActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorScale3D(ScaleFactor);

	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetLinearDamping(RollingLinearDamping);
	StaticMesh->SetAngularDamping(RollingAngularDamping);
	StaticMesh->SetMassOverrideInKg(NAME_None, RollingMassInKg);

	StaticMesh->BodyInstance.bOverrideMaxAngularVelocity = true;
	StaticMesh->BodyInstance.MaxAngularVelocity = MaxRollingAngularVelocity;

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

	LastKnownVelocity = StaticMesh->GetPhysicsLinearVelocity();

	const float Distance = FVector::DistSquared(StartLocation, GetActorLocation());
	if (Distance >= FMath::Square(MaxDistance))
	{
		BreakAndDestroy();
	}
}

void ARollingActor::LaunchInDirection(const FVector& Direction)
{
	if (AActor* SkillOwner = GetOwner())
	{
		UMonsterSkillComponent* SkillComponent = SkillOwner->FindComponentByClass<UMonsterSkillComponent>();
		if (SkillComponent)
		{
			const FMonsterSkillData& Data = SkillComponent->GetCurrentSkillData();
			Damage = Data.Damage;
		}
	}

	if (!CollisionComp) return;

	StartLocation = GetActorLocation();

	FVector LaunchDir = Direction.GetSafeNormal();
	LaunchDir.Z = -0.2f;
	LaunchDir = LaunchDir.GetSafeNormal();

	const FVector Velocity = LaunchDir * LaunchSpeed;
	StaticMesh->SetPhysicsLinearVelocity(Velocity);
}

void ARollingActor::OnRollingOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,	const FHitResult& SweepResult)
{
	if (!Other || Other == this || Other == GetOwner() || !OtherComp) return;
	if (bHasBroken) return;

	UGameplayStatics::ApplyDamage(
		Other,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);
}

void ARollingActor::BreakAndDestroy()
{
	if (!IsValid(this) || bHasBroken) return;
	bHasBroken = true;

	GetWorld()->GetTimerManager().ClearTimer(BreakTimerHandle);

	if (!BreakGeometryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BreakGeometryClass is null!"));
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

	const FVector AngularVel = StaticMesh->GetPhysicsAngularVelocityInDegrees();
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
