#include "MonsterAI/Skills/RotatingProjectile.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ARotatingProjectile::ARotatingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetGenerateOverlapEvents(false);
	CollisionComp->SetSimulatePhysics(false);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetGenerateOverlapEvents(false);

	LandingTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LandingTrigger"));
	LandingTrigger->SetupAttachment(RootComp);
	LandingTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LandingTrigger->SetGenerateOverlapEvents(false);
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

	if (!bIsLaunched) return;

	if (!bHasLanded)
	{
		if (bIsParabolic)
		{
			UpdateParabolicMovement(DeltaTime);
		}
		else
		{
			const FVector DeltaMove = MoveDirection * MoveSpeed * DeltaTime;
			AddActorWorldOffset(DeltaMove, true);
		}

		FRotator Spin(-RotatingSpeed * DeltaTime, 0.f, 0.f);
		RootComp->AddLocalRotation(Spin);

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
	}

	if (bHasLanded && bDestroyOnBossApproach && BossActor)
	{
		const float Distance = FVector::DistSquared(GetActorLocation(), BossActor->GetActorLocation());
		if (Distance < FMath::Square(BossApproachRadius))
		{
			UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] Destroy!"));
			Destroy();
		}
	}
}

void ARotatingProjectile::SetBossActor(AActor* InBoss, FName InSocket)
{
	BossActor = InBoss;
	RestoreSocketName = InSocket;
}

void ARotatingProjectile::LaunchProjectile(const FVector& InTargetLocation, float Speed)
{
	AlreadyHitActors.Empty();
	bIsLaunched = true;
	TargetLocation = InTargetLocation;

	if (bIsParabolic)
	{
		InitialPosition = GetActorLocation();
		InitialVelocity = ComputeParabolicVelocity(InitialPosition, TargetLocation, Speed);
		TimeSinceLaunch = 0.f;
	}
	else
	{
		MoveDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
		MoveSpeed = Speed;
	}

	SetActorRotation(FRotator(0.f, MoveDirection.Rotation().Yaw, 0.f));
	SetActorScale3D(FVector(1.5f));
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComp->SetGenerateOverlapEvents(true);

	LandingTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LandingTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	LandingTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	LandingTrigger->SetGenerateOverlapEvents(true);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ARotatingProjectile::OnOverlap);
	LandingTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARotatingProjectile::OnLandingDetected);

	UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] LaunchProjectile :  %s throws %s"), *BossActor->GetName(), *this->GetName());
}

FVector ARotatingProjectile::ComputeParabolicVelocity(const FVector& Start, const FVector& Target, float Speed) const
{
	FVector Delta = Target - Start;
	FVector DeltaXY = FVector(Delta.X, Delta.Y, 0.f);
	float DistXY = DeltaXY.Size();
	float Height = Delta.Z;
	float Gravity = -GravityZ;

	float SpeedSq = Speed * Speed;
	float Discriminant = SpeedSq * SpeedSq - Gravity * (Gravity * DistXY * DistXY + 2 * Height * SpeedSq);

	if (Discriminant < 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Projectile] Target unreachable. Default to flat shot."));
		return Delta.GetSafeNormal() * Speed;
	}

	float Root = FMath::Sqrt(Discriminant);
	float Angle = FMath::Atan((SpeedSq - Root) / (Gravity * DistXY));

	FVector DirXY = DeltaXY.GetSafeNormal();

	FVector ResultVelocity =
		DirXY * Speed * FMath::Cos(Angle) +
		FVector::UpVector * Speed * FMath::Sin(Angle);

	return ResultVelocity;
}

void ARotatingProjectile::UpdateParabolicMovement(float DeltaTime)
{
	TimeSinceLaunch += DeltaTime;

	FVector Gravity = FVector(0, 0, GravityZ);
	FVector NewLocation =
		InitialPosition +
		InitialVelocity * TimeSinceLaunch +
		0.5f * Gravity * FMath::Square(TimeSinceLaunch);

	SetActorLocation(NewLocation, true);
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
}

void ARotatingProjectile::OnLandingDetected(UPrimitiveComponent* OverlappedComp, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bHasLanded && Other && Other != this && OtherComp)
	{
		const ECollisionChannel HitChannel = OtherComp->GetCollisionObjectType();
		if (HitChannel == ECC_WorldStatic || HitChannel == ECC_WorldDynamic)
		{
			HandleLanding();
		}
	}
}

void ARotatingProjectile::HandleLanding()
{
	if (bHasLanded) return;

	bHasLanded = true;
	MoveSpeed = 0.f;
	StaticMeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	FTimerHandle CollisionUpdateTimer;
	GetWorld()->GetTimerManager().SetTimer(
		CollisionUpdateTimer,
		this,
		&ARotatingProjectile::UpdateLandingCollision,
		1.0f,
		false
	);

	if (!bDestroyOnBossApproach)
	{
		SetLifeSpan(AutoDestroyDelay);
	}

	UE_LOG(LogTemp, Warning, TEXT("[RotatingProjectile] Stopped."));
}

void ARotatingProjectile::UpdateLandingCollision()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetGenerateOverlapEvents(false);
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
