#include "ProjectileBomb.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "FriendlyAI/AnimalMonster.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"


AProjectileBomb::AProjectileBomb()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBomb::OnHit);

	NiagaraComp->SetupAttachment(RootComp);
	StaticMesh->SetupAttachment(RootComp);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComp;
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->MaxSpeed = 1200.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->PrimaryComponentTick.bCanEverTick = true;
	ProjectileMovement->PrimaryComponentTick.bStartWithTickEnabled = true;
	ProjectileMovement->SetComponentTickEnabled(true);
	ProjectileMovement->bUpdateOnlyIfRendered = false;

	ExplosionOverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionOverlapComp"));
	ExplosionOverlapComp->SetupAttachment(CollisionComp);
	ExplosionOverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionOverlapComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	ExplosionOverlapComp->SetGenerateOverlapEvents(false);
	ExplosionOverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBomb::OnExplosionOverlap);
	
	InitialLifeSpan = ActorLifeTime;
}

void AProjectileBomb::BeginPlay()
{
	Super::BeginPlay();

	if (LaunchDelayTime > KINDA_SMALL_NUMBER)
	{
		if (bHomingActive)
		{
			GetWorldTimerManager().SetTimer(
				HomingTimerHandle,
				this,
				&AProjectileBomb::ActivateHoming,
				LaunchDelayTime,
				false
			);
		}
		else
		{
			GetWorldTimerManager().SetTimer(
			LaunchTimerHandle,
			this,
			&AProjectileBomb::LaunchProjectile,
			LaunchDelayTime,
			false
			);
		}
	}
	else
	{
		bHomingActive ? ActivateHoming() : LaunchProjectile();
	}
}

void AProjectileBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bHomingActive || !IsValid(HomingTarget)) return;
	if (!IsValid(SpawnActorClass)) return;
	
	const float CurrentDist = FVector::Dist(GetActorLocation(), HomingTarget->GetActorLocation());
	if (CurrentDist <= SpawnDistance)
	{
		if (SpawnActorClass)
		{
			const FTransform Tf(GetActorRotation(), GetActorLocation());
			GetWorld()->SpawnActor<AActor>(SpawnActorClass, Tf);
		}
		
		Destroy();
	}
}

void AProjectileBomb::LaunchProjectile()
{
	if (!ProjectileMovement) return;
	
	const float PitchRad = FMath::DegreesToRadians(LaunchPitchAngle);

	FVector Forward = GetActorForwardVector();
	FVector HorizontalDir = Forward;
	HorizontalDir.Z = 0.f;
	if (HorizontalDir.IsNearlyZero())
	{
		HorizontalDir = FVector::ForwardVector;
	}
	HorizontalDir.Normalize();

	FVector LaunchDir = HorizontalDir * FMath::Cos(PitchRad) + FVector::UpVector * FMath::Sin(PitchRad);
	LaunchDir.Normalize();
	
	const FVector LaunchVel = LaunchDir * ProjectileMovement->InitialSpeed;
	
	ProjectileMovement->Velocity = LaunchVel;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
	ProjectileMovement->Activate(true);
}

void AProjectileBomb::ActivateHoming()
{
	if (!ProjectileMovement || !bHomingActive) return;

	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!IsValid(PlayerPawn)) return;

	HomingTarget = PlayerPawn;
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingTargetComponent = PlayerPawn->GetRootComponent();
	ProjectileMovement->HomingAccelerationMagnitude = HomingSpeed;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
	ProjectileMovement->Activate(true);
}

float AProjectileBomb::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	if (!bCanBeDestroyed) return 0.f;

	TArray<AActor*> OverlappingActors;
	CollisionComp->GetOverlappingActors(OverlappingActors);
	if (OverlappingActors.Num() > 0) return 0.f;
	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}
	
	CurrentHealth -= ActualDamage;
	if (CurrentHealth <= 0.f)
	{
		Destroy();
	}

	return ActualDamage;
}

void AProjectileBomb::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(AProjectileBomb::StaticClass())) return;
	
	if (!OtherComp || OtherActor == this || OtherActor == GetInstigator() || Cast<ABaseMonster>(OtherActor) || Cast<AAnimalMonster>(OtherActor))
		return;

	const FVector ImpactPoint = Hit.ImpactPoint;
	
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ExplosionEffect,
			ImpactPoint,
			FRotator::ZeroRotator,
			ExplosionScale
		);
	}
	
	ExplosionOverlapComp->SetSphereRadius(ExplosionRadius);
	ExplosionOverlapComp->SetGenerateOverlapEvents(true);
	
	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}
	
	SetActorLocation(ImpactPoint);
	SetActorRotation(Hit.ImpactNormal.Rotation());
	
	if (CollisionComp->IsRegistered())
		CollisionComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(ActorLifeTime);
}

void AProjectileBomb::OnExplosionOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AProjectileBomb::StaticClass())) return;
	
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator() || Cast<ABaseMonster>(OtherActor) || Cast<AAnimalMonster>(OtherActor))
		return;

	FTimerHandle& Handle = OverlapDamageTimers.FindOrAdd(OtherActor);
	GetWorldTimerManager().SetTimer(
		Handle,
		FTimerDelegate::CreateUObject(this, &AProjectileBomb::ApplyPeriodicDamage, OtherActor),
		0.4f,
		true
	);
}

void AProjectileBomb::ApplyPeriodicDamage(AActor* Victim)
{
	if (!Victim) return;

	UGameplayStatics::ApplyDamage(
		Victim,
		ExplosionDamage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);
}
