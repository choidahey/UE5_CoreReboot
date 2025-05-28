#include "MonsterAI/Skills/ColdFairyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Kismet/KismetMathLibrary.h"

AColdFairyActor::AColdFairyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(20.f);
	CollisionComp->SetupAttachment(RootComp);
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AColdFairyActor::OnHit);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	MovementComp->bAutoActivate = false;
	MovementComp->bIsHomingProjectile = false;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->HomingAccelerationMagnitude = 5000.f;
	MovementComp->InitialSpeed = Speed;
	MovementComp->MaxSpeed = MaxSpeed;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bSweepCollision  = true;
	MovementComp->UpdatedComponent = CollisionComp; 
}

void AColdFairyActor::BeginPlay()
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
}

void AColdFairyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsValid(TargetActorPtr))
	{
		FVector ToTarget = (TargetActorPtr->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		LaunchDirection = ToTarget;
		FRotator LookAtRot = ToTarget.Rotation();
		LookAtRot.Roll = 0.f;
		SetActorRotation(LookAtRot);
	}
}

void AColdFairyActor::Launch(AActor* TargetActor)
{
	TargetActorPtr = TargetActor;

	if (APawn* InstPawn = GetInstigator<APawn>())
	{
		CollisionComp->IgnoreActorWhenMoving(InstPawn, true);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
			DisableTickTimerHandle,
			FTimerDelegate::CreateLambda([this]()
			{
				SetActorTickEnabled(false);
				
				if (!TargetActorPtr || !MovementComp) return;

				FVector Dir = (TargetActorPtr->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				LaunchDirection = Dir;
				
				FRotator LookAtRot = Dir.Rotation();
				LookAtRot.Roll = 0.f;
				SetActorRotation(LookAtRot);

				MovementComp->Velocity = Dir * MovementComp->InitialSpeed;
				MovementComp->Activate(true);
			}),
			1.0f,
			false
		);
}

void AColdFairyActor::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);
	
	MovementComp->StopMovementImmediately();
	SetActorLocation(Hit.ImpactPoint);
	AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);

	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		FTimerDelegate::CreateLambda([this]() { Destroy(); }),
		2.0f,
		false
	);
}
