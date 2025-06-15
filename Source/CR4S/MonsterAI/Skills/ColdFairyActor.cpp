#include "MonsterAI/Skills/ColdFairyActor.h"

#include "CR4S.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MonsterAI/BaseMonster.h"

AColdFairyActor::AColdFairyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	if (USphereComponent* Sphere = Cast<USphereComponent>(CollisionComp))
	{
		Sphere->SetSphereRadius(20.f);
		Sphere->SetupAttachment(RootComp);
	}
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AColdFairyActor::OnHit);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMesh->SetupAttachment(RootComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = Speed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	ProjectileMovementComp->ProjectileGravityScale = 1.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bAutoActivate = false;
	ProjectileMovementComp->bIsHomingProjectile = false;
	ProjectileMovementComp->HomingAccelerationMagnitude = 0.f;
}

void AColdFairyActor::BeginPlay()
{
	Super::BeginPlay();

	CR4S_Log(LogDa, Warning, TEXT("[%s] BeginPlay - Damage : %f"), *MyHeader, Damage);
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

void AColdFairyActor::InitialLaunch(AActor* InTarget, int32 InIndex, int32 InTotalCount)
{
	if (!InTarget) return;
	
	TargetActor = InTarget;
	SpawnOrder = InIndex;
	TotalCount = InTotalCount;

	if (APawn* InstPawn = GetInstigator<APawn>())
		CollisionComp->IgnoreActorWhenMoving(InstPawn, true);
	
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

void AColdFairyActor::Launch()
{
	if (!CR4S_VALIDATE(LogDa, IsValid(this))) return;
	if (!CR4S_VALIDATE(LogDa, IsValid(TargetActor))) return;
	if (bHasLaunched) return;

	bHasLaunched = true;
	
	if (!ProjectileMovementComp->IsActive())
		ProjectileMovementComp->Activate(true);
	
	ProjectileMovementComp->bIsHomingProjectile = true;
	ProjectileMovementComp->HomingAccelerationMagnitude = 5000.f;
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

void AColdFairyActor::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	
    ProjectileMovementComp->StopMovementImmediately();
    SetActorLocation(Hit.ImpactPoint);

    if (CollisionComp->IsRegistered())
        CollisionComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    
    AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);

    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimerHandle,
        FTimerDelegate::CreateLambda([this]() { Destroy(); }),
        2.0f,
        false
    );
}
