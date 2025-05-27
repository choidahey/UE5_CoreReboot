#include "MonsterAI/Skills/ColdFairyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Kismet/KismetMathLibrary.h"

AColdFairyActor::AColdFairyActor()
	: MyHeader(TEXT("ColdFairyActor"))
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(20.f);
	CollisionComp->SetCollisionProfileName("OverlapAll");
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetupAttachment(RootComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	MovementComp->bAutoActivate = false;
	MovementComp->bIsHomingProjectile = false;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->HomingAccelerationMagnitude = 5000.f;
	MovementComp->InitialSpeed = 5000.f;
	MovementComp->MaxSpeed = 5000.f;
	MovementComp->ProjectileGravityScale = 0.f;

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AColdFairyActor::OnHit);
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

	APawn* TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!TargetPlayer) return;

	FVector ToTarget = (TargetPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	LaunchDirection = ToTarget;
	FRotator LookAtRot = ToTarget.Rotation();
	LookAtRot.Roll = 0.f;
	SetActorRotation(LookAtRot);
}

void AColdFairyActor::Launch()
{
	SetActorTickEnabled(false);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player || !MovementComp)
		return;

	FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	LaunchDirection = Dir;

	FRotator LookAtRot = Dir.Rotation();
	LookAtRot.Roll = 0.f;
	SetActorRotation(LookAtRot);

	MovementComp->Velocity = Dir * MovementComp->InitialSpeed;
	MovementComp->Activate(true);
}

void AColdFairyActor::OnHit(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	if (Other && Other != this)
	{
		UGameplayStatics::ApplyDamage(Other, Damage, GetInstigatorController(), this, nullptr);
		Destroy();
	}
}
