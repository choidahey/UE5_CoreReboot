#include "MonsterAI/Skills/RotatingProjectile.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ARotatingProjectile::ARotatingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetGenerateOverlapEvents(false);
	CollisionComp->SetSimulatePhysics(false);
	RootComponent = CollisionComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetGenerateOverlapEvents(false);
	StaticMeshComp->SetupAttachment(CollisionComp);
}

void ARotatingProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		if (UMonsterSkillComponent* SkillComp = OwnerActor->FindComponentByClass<UMonsterSkillComponent>())
		{
			const FMonsterSkillData& SkillData = SkillComp->GetCurrentSkillData();
			Damage = SkillData.Damage;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] BeginPlay - Damage : %f"), Damage);
}

void ARotatingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHasLanded)
	{
		FRotator Spin(-RotatingSpeed * DeltaTime, 0.f, 0.f);
		CollisionComp->AddLocalRotation(Spin);

		const FVector DeltaMove = MoveDirection * MoveSpeed * DeltaTime;
		AddActorWorldOffset(DeltaMove, true);
	}

	// Debug Capsule
	DrawDebugCapsule(
		GetWorld(),
		CollisionComp->GetComponentLocation(),
		CollisionComp->GetScaledCapsuleHalfHeight(),
		CollisionComp->GetScaledCapsuleRadius(),
		CollisionComp->GetComponentQuat(),
		FColor::Green,
		false, -1.f, 0,
		1.5f
	);

	if (bDestroyOnBossApproach && BossActor)
	{
		const float Distance = FVector::DistSquared(GetActorLocation(), BossActor->GetActorLocation());
		if (Distance < FMath::Square(BossApproachRadius))
		{
			UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] Destroy!"));
			Destroy();
		}
	}
}

void ARotatingProjectile::Destroyed()
{
	if (!IsValid(BossActor)) return;

	USkeletalMeshComponent* Mesh = BossActor->FindComponentByClass<USkeletalMeshComponent>();
	if (Mesh && Mesh->DoesSocketExist(RestoreSocketName))
	{
		const FName BoneName = Mesh->GetSocketBoneName(RestoreSocketName);
		Mesh->UnHideBoneByName(BoneName);
	}

	Super::Destroyed();
}

void ARotatingProjectile::LaunchProjectile(const FVector& Direction, float Speed)
{
	AlreadyHitActors.Empty();

	MoveDirection = Direction.GetSafeNormal();
	MoveSpeed = Speed;

	SetActorRotation(FRotator(0.f, MoveDirection.Rotation().Yaw, 0.f));
	SetActorScale3D(FVector(1.f));
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComp->SetGenerateOverlapEvents(true);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARotatingProjectile::OnOverlap);

	UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] LaunchProjectile :  %s throws %s"), *BossActor->GetName(), *this->GetName());
}

void ARotatingProjectile::SetBossActor(AActor* InBoss, FName InSocket)
{
	BossActor = InBoss;
	RestoreSocketName = InSocket;
}

void ARotatingProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(Other) || Other == this || Other == BossActor) return;

	if (AlreadyHitActors.Contains(Other)) return;
	AlreadyHitActors.Add(Other);

	UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] OnOverlap : ApplyDamage %.1f! to %s"), Damage, *Other->GetName());

	UGameplayStatics::ApplyDamage(
		Other,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	if (Other->IsA(APawn::StaticClass()))
	{
		return;
	}

	const ECollisionChannel HitType = OtherComp->GetCollisionObjectType();
	if (HitType == ECC_WorldStatic || HitType == ECC_WorldDynamic)
	{
		HandleLanding(Other);
	}
}

void ARotatingProjectile::HandleLanding(AActor* HitActor)
{
	if (bHasLanded) return;

	bHasLanded = true;
	MoveSpeed = 0.f;
	StaticMeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetGenerateOverlapEvents(false);

	if (!bDestroyOnBossApproach)
	{
		SetLifeSpan(AutoDestroyDelay);
	}

	UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] Stopped on %s"), *HitActor->GetName());
}
