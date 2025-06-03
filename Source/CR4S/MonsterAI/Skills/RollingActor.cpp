#include "MonsterAI/Skills/RollingActor.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

ARollingActor::ARollingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	SetRootComponent(CollisionComp);

	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARollingActor::OnRollingOverlap);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(CollisionComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetSimulatePhysics(false);
}

void ARollingActor::BeginPlay()
{
	Super::BeginPlay();

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

	const FVector Velocity = Direction * LaunchSpeed;
	CollisionComp->SetPhysicsLinearVelocity(Velocity);
}

void ARollingActor::OnRollingOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,	const FHitResult& SweepResult)
{
	if (!Other || Other == this || Other == GetOwner() || !OtherComp) return;

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
	if (!IsValid(this)) return;

	GetWorld()->GetTimerManager().ClearTimer(BreakTimerHandle);

	if (!BreakGeometryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BreakGeometryClass is null!"));
		return;
	}

	if (BreakGeometryClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FTransform SpawnTransform = StaticMesh->GetComponentTransform();

		AGeometryCollectionActor* BreakActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(
			BreakGeometryClass, SpawnTransform, Params);

		if (BreakActor && BreakActor->GetGeometryCollectionComponent())
		{
			UGeometryCollectionComponent* GC = BreakActor->GetGeometryCollectionComponent();

			const FVector ForwardDir = CollisionComp->GetPhysicsLinearVelocity().GetSafeNormal();
			const FVector Impulse = ForwardDir * 1000.f + FVector(0, 0, 300.f);

			GC->SetSimulatePhysics(true);
			GC->AddImpulse(Impulse, NAME_None, true);

			BreakActor->SetLifeSpan(5.0f);
		}
	}

	Destroy();
}