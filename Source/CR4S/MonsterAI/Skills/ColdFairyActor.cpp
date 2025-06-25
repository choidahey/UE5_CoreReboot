#include "MonsterAI/Skills/ColdFairyActor.h"
#include "CR4S.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AColdFairyActor::AColdFairyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AColdFairyActor::OnOverlap);

	NiagaraComp->SetupAttachment(RootComp);
	
	StaticMesh->SetupAttachment(RootComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->UpdatedComponent = RootComp;
	ProjectileMovementComp->InitialSpeed = Speed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	ProjectileMovementComp->ProjectileGravityScale = 1.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bAutoActivate = false;
	ProjectileMovementComp->bIsHomingProjectile = false;
	ProjectileMovementComp->HomingAccelerationMagnitude = 0.f;
	ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &AColdFairyActor::OnProjectileStop);
}

void AColdFairyActor::BeginPlay()
{
	Super::BeginPlay();

	CR4S_Log(LogDa, Warning, TEXT("[%s] BeginPlay - Damage : %f"), *MyHeader, Damage);

	if (IsValid(CollisionComp) && CollisionComp->IsRegistered())
	{
		if (APawn* InstPawn = GetInstigator<APawn>())
			CollisionComp->IgnoreActorWhenMoving(InstPawn, true);
	}
}

void AColdFairyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bHasLaunched && IsValid(TargetActor))
	{
		FVector ToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		if (!ToTarget.IsNearlyZero())
		{
			SetActorRotation(ToTarget.Rotation());
		}
	}
}

void AColdFairyActor::OnProjectileStop(const FHitResult& ImpactResult)
{
	StopAndStick(ImpactResult);
}

void AColdFairyActor::InitialLaunch(AActor* InTarget, int32 InIndex, int32 InTotalCount)
{
	if (!InTarget) return;
	
	TargetActor = InTarget;
	SpawnOrder = InIndex;
	TotalCount = InTotalCount;

	bSequentialLaunch ? HandleSequenceLaunch() : HandleImmediateLaunch();
}

void AColdFairyActor::HandleSequenceLaunch()
{
	LaunchDelay = SpawnOrder * Interval;

	if (LaunchDelay <= KINDA_SMALL_NUMBER)
	{
		Launch();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			LaunchTimerHandle,
			this,
			&AColdFairyActor::Launch,
			LaunchDelay,
			false
		);
	}
}

void AColdFairyActor::HandleImmediateLaunch() const
{
	if (bHasLaunched) return;
	
	TArray<AActor*> SpawnedActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AColdFairyActor::StaticClass(), SpawnedActors);

	TArray<AColdFairyActor*> FairyActors;
	for (AActor* Actor : SpawnedActors)
	{
		AColdFairyActor* CF = Cast<AColdFairyActor>(Actor);
		if (CF && CF->GetOwner() == GetOwner() && !CF->bHasLaunched)
			FairyActors.Add(CF);
	}
    	
	if (FairyActors.Num() < TotalCount) return;
	float DelayTime = FairyActors.Last()->LaunchDelay;
    	
	if (DelayTime <= KINDA_SMALL_NUMBER)
	{
		for (AColdFairyActor* FairyActor : FairyActors)
		{
			if (IsValid(FairyActor))
				FairyActor->Launch();
		}
	}
	else
	{
		FTimerDelegate LaunchAllDelegate;
		LaunchAllDelegate.BindLambda([FairyActors]()
		{
			for (AColdFairyActor* FairyActor : FairyActors)
			{
				if (IsValid(FairyActor))
					FairyActor->Launch();
			}
		});
		
		GetWorld()->GetTimerManager().SetTimer(
		FairyActors.Last()->LaunchTimerHandle,
			LaunchAllDelegate,
			DelayTime,
			false
		);
	}
}

void AColdFairyActor::StopAndStick(const FHitResult& HitResult, AActor* HitActor, UPrimitiveComponent* HitComp)
{
	if (!ProjectileMovementComp->IsActive()) return;
	
	ProjectileMovementComp->StopMovementImmediately();
	ProjectileMovementComp->Deactivate();
	ProjectileMovementComp->bIsHomingProjectile = false;
	ProjectileMovementComp->Velocity = FVector::ZeroVector;
	
	if (IsValid(HitComp) && HitComp != CollisionComp)
	{
		AttachToComponent(HitComp, FAttachmentTransformRules::KeepWorldTransform);
	}
	else if (IsValid(HitActor) && HitComp != CollisionComp)
	{
		AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform);
	}
	
	if (IsValid(CollisionComp) && CollisionComp->IsRegistered())
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionComp->SetGenerateOverlapEvents(false);
	}
	
	FTimerDelegate DestroyDelegate;
	DestroyDelegate.BindLambda([this]() { Destroy(); });
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, LifeTime, false);
}

void AColdFairyActor::Launch()
{
	if (!CR4S_VALIDATE(LogDa, IsValid(this))) return;
	if (!CR4S_VALIDATE(LogDa, IsValid(TargetActor))) return;
	if (bHasLaunched) return;

	bHasLaunched = true;
	SetActorTickEnabled(false);
	
	if (!ProjectileMovementComp->IsActive())
		ProjectileMovementComp->Activate(true);

	FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	if (Direction.IsNearlyZero()) return;
	
	SetActorRotation(Direction.Rotation());

	ProjectileMovementComp->InitialSpeed = Speed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	
	if (bUseHoming)
	{
		ProjectileMovementComp->ProjectileGravityScale = GravityScale;
		ProjectileMovementComp->bIsHomingProjectile = bUseHoming;
		ProjectileMovementComp->HomingAccelerationMagnitude = HomingAcceleration ;
		ProjectileMovementComp->HomingTargetComponent = TargetActor->GetRootComponent();
		ProjectileMovementComp->bRotationFollowsVelocity = true;
	}
	else
	{
		ProjectileMovementComp->ProjectileGravityScale = 0.1f;
		ProjectileMovementComp->bIsHomingProjectile = false;
		ProjectileMovementComp->HomingAccelerationMagnitude = 2000.f;
		ProjectileMovementComp->HomingTargetComponent = nullptr;
		ProjectileMovementComp->bRotationFollowsVelocity = true; 
	}
	ProjectileMovementComp->Velocity = Direction * Speed;
	ProjectileMovementComp->UpdateComponentVelocity();
}

void AColdFairyActor::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherComp) return;
	if (OtherActor->IsA<AColdFairyActor>() || OtherActor->IsA(ABaseSkillActor::StaticClass())) return;
	if (OtherActor && OtherActor == GetInstigator()) return;
	
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	StopAndStick(SweepResult, OtherActor, OtherComp);
}

void AColdFairyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(LaunchTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(LaunchTimerHandle);
		}
        
		if (GetWorld()->GetTimerManager().IsTimerActive(DestroyTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(DestroyTimerHandle);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}
 