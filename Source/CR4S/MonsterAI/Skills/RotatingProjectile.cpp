#include "MonsterAI/Skills/RotatingProjectile.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/CapsuleComponent.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Region/KamishForestBoss.h"

ARotatingProjectile::ARotatingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetGenerateOverlapEvents(false);
	CollisionComp->SetSimulatePhysics(false);

	LandingTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LandingTrigger"));
	LandingTrigger->SetupAttachment(RootComp);
	LandingTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LandingTrigger->SetGenerateOverlapEvents(false);

	//TrailEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	//TrailEffectComp->SetupAttachment(StaticMesh);
	//TrailEffectComp->SetAutoActivate(false);
}

void ARotatingProjectile::BeginPlay()
{
	Super::BeginPlay();

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

		const FRotator Spin(-RotatingSpeed * DeltaTime, 0.f, 0.f);
		RootComp->AddLocalRotation(Spin);
	}

	if (bHasLanded && bDestroyOnBossApproach && BossActor)
	{
		const float Distance = FVector::DistSquared(GetActorLocation(), BossActor->GetActorLocation());
		if (Distance < FMath::Square(BossApproachRadius))
		{
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
	bIsLaunched = true;
	TargetLocation = InTargetLocation;
	TimeSinceLaunch = 0.f;

	if (bIsParabolic)
	{
		InitialPosition = GetActorLocation();
		InitialVelocity = ComputeParabolicVelocity(InitialPosition, TargetLocation, Speed);
	}
	else
	{
		MoveDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
		MoveSpeed = Speed;
	}

	SetActorRotation(FRotator(0.f, MoveDirection.Rotation().Yaw, 0.f));
	SetActorScale3D(ProjectileScale);
	
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);

	LandingTrigger->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	LandingTrigger->SetGenerateOverlapEvents(true);

	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ARotatingProjectile::OnOverlap);
	LandingTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &ARotatingProjectile::OnLandingDetected);

	//if (TrailEffect && TrailEffectComp)
	//{
	//	TrailEffectComp->SetAsset(TrailEffect);
	//	TrailEffectComp->Activate(true);
	//}

	if (TrailEffect && !TrailEffectComp)
	{
		TrailEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailEffect,
			StaticMesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
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

void ARotatingProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ARotatingProjectile::OnLandingDetected(UPrimitiveComponent* OverlappedComp, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bHasLanded && Other && Other != this && OtherComp)
	{
		const ECollisionChannel HitChannel = OtherComp->GetCollisionObjectType();
		if (HitChannel == ECC_WorldStatic)
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

	if (TrailEffectComp && TrailEffectComp->IsActive())
	{
		// TrailEffectComp->Deactivate();
		TrailEffectComp->DestroyComponent();
	}

	if (AKamishForestBoss* Boss = Cast<AKamishForestBoss>(GetOwner()))
	{
		Boss->SetWeaponLandingLocation(GetActorLocation());
		UE_LOG(LogTemp, Log, TEXT("[Projectile] Landing location sent to boss: %s"), *GetActorLocation().ToString());
	}

	FTimerHandle CollisionUpdateTimer;
	GetWorld()->GetTimerManager().SetTimer(
		CollisionUpdateTimer,
		this,
		&ARotatingProjectile::UpdateLandingCollision,
		1.0f,
		false
	);

	SetLifeSpan(AutoDestroyDelay);
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
	if (Mesh && Mesh->DoesSocketExist(RestoreSocketName) && VisualWeaponClass)
	{
		FVector Location = Mesh->GetSocketLocation(RestoreSocketName);
		FRotator Rotation = Mesh->GetSocketRotation(RestoreSocketName);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Owner = BossActor;

		AActor* VisualWeapon = GetWorld()->SpawnActor<AActor>(VisualWeaponClass, Location, Rotation, Params);
		if (VisualWeapon)
		{
			VisualWeapon->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, RestoreSocketName);
		}
	}

	Super::Destroyed();
}
