#include "MonsterAI/Skills/ColdFairyActor.h"
#include "CR4S.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AColdFairyActor::AColdFairyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
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
	CR4S_Log(LogDa, Log, TEXT("[%s] OnProjectileStop - Hit: %s"), *MyHeader, *ImpactResult.GetActor()->GetName());
	
	StopAndStick(ImpactResult);
}

void AColdFairyActor::InitialLaunch(AActor* InTarget, int32 InIndex, int32 InTotalCount)
{
	if (!InTarget) return;
	
	TargetActor = InTarget;
	SpawnOrder = InIndex;
	TotalCount = InTotalCount;

	// bSequentialLaunch ? HandleSequenceLaunch() : HandleImmediateLaunch();
	bSequentialLaunch ? HandleSequenceLaunch() : Launch();
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
	TArray<AActor*> SpawnedActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AColdFairyActor::StaticClass(), SpawnedActors);

	TArray<AColdFairyActor*> FairyActors;
	for (AActor* Actor : SpawnedActors)
	{
		AColdFairyActor* CF = Cast<AColdFairyActor>(Actor);
		if (CF && CF->GetOwner() == GetOwner())
			FairyActors.Add(CF);
	}
    	
	if (FairyActors.Num() < TotalCount) return;
    	
	for (AColdFairyActor* FairyActor : FairyActors)
	{
		if (IsValid(FairyActor))
			FairyActor->Launch();
	}
}

void AColdFairyActor::StopAndStick(const FHitResult& HitResult, AActor* HitActor, UPrimitiveComponent* HitComp)
{
	if (!ProjectileMovementComp->IsActive()) return;
	
	ProjectileMovementComp->StopMovementImmediately();
	ProjectileMovementComp->Deactivate();
	ProjectileMovementComp->bIsHomingProjectile = false;
	ProjectileMovementComp->Velocity = FVector::ZeroVector;
	
	SetActorLocation(HitResult.ImpactPoint);
	
	if (IsValid(CollisionComp) && CollisionComp->IsRegistered())
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionComp->SetGenerateOverlapEvents(false);
	}
	
	if (IsValid(HitComp) && HitComp != CollisionComp)
	{
		CollisionComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);  // 여기 터짐
		CollisionComp->AttachToComponent(HitComp,FAttachmentTransformRules::KeepWorldTransform);
	}
	else if (IsValid(HitActor) && HitComp != CollisionComp)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform);
	}
	
	FTimerDelegate DestroyDelegate;
	DestroyDelegate.BindLambda([this]() 
	{ 
		CR4S_Log(LogDa, Log, TEXT("[%s] LifeTime expired, destroying actor"), *MyHeader);
		Destroy(); 
	});
	
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, LifeTime, false);
}

void AColdFairyActor::Launch()
{
	if (!CR4S_VALIDATE(LogDa, IsValid(this))) return;
	if (!CR4S_VALIDATE(LogDa, IsValid(TargetActor))) return;
	if (bHasLaunched) return;

	bHasLaunched = true;
	
	if (!ProjectileMovementComp->IsActive())
		ProjectileMovementComp->Activate(true);
	
	ProjectileMovementComp->bIsHomingProjectile = true;
	ProjectileMovementComp->ProjectileGravityScale = 0.3f; 
	ProjectileMovementComp->HomingAccelerationMagnitude = 2000.f;
	ProjectileMovementComp->HomingTargetComponent = TargetActor->GetRootComponent();
	
	FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		CR4S_Log(LogDa, Warning, TEXT("[%s][LaunchSelf] Direction is nearly zero. Aborting."), *GetName());
		return;
	}

	FRotator NewRot = Direction.Rotation();
	SetActorRotation(NewRot);
	ProjectileMovementComp->Velocity = Direction * ProjectileMovementComp->InitialSpeed;
	
	CR4S_Log(LogDa, Log,
		TEXT("[%s] Launch: Homeing activated → Target=%s, InitialDir=%s"),
		*MyHeader,
		*TargetActor->GetName(),
		*Direction.ToString()
	);
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
	if (OtherActor && OtherActor->IsA<AColdFairyActor>()) return;
	
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	StopAndStick(SweepResult, OtherActor, OtherComp);
}

