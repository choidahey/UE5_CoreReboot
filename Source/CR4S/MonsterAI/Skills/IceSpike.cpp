#include "MonsterAI/Skills/IceSpike.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "CR4S.h"

AIceSpike::AIceSpike()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComponent);
	if (UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(CollisionComp))
	{
		CapsuleComp->InitCapsuleSize(50.f, 50.f);
	}
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIceSpike::OnOverlap);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InitialLifeSpan = LifeTime;
}

void AIceSpike::BeginPlay()
{
	Super::BeginPlay();
	
	CR4S_Log(LogDa, Warning, TEXT("[%s] BeginPlay - Damage : %f"), *MyHeader, Damage);
}
void AIceSpike::Launch()
{
	if (!StaticMeshComp || !CollisionComp) return;
	
	InitialOffset = StaticMeshComp->GetRelativeLocation();
	TargetOffset = InitialOffset + FVector(0, 0, 0);
	ElapsedRise = 0.f;
	bRising = true;
}

void AIceSpike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bRising) return;

	ElapsedRise += DeltaTime;
	const float Alpha = FMath::Clamp(ElapsedRise / RiseDuration, 0.f, 1.f);

	const FVector NewOffset = FMath::Lerp(InitialOffset, TargetOffset, Alpha);
	StaticMeshComp->SetRelativeLocation(NewOffset);
	CollisionComp->SetRelativeLocation(NewOffset);

	if (Alpha >= 1.f)
		bRising = false;
}

void AIceSpike::OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
{
	if (ACharacter* Char = Cast<ACharacter>(OtherActor))
	{
		const FVector Dir = (Char->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		const FVector LaunchVel = Dir * PushHorizontalStrength + FVector(0, 0, PushUpStrength);
		Char->LaunchCharacter(LaunchVel, true, true);
	}

	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}


