#include "MonsterAI/Skills/IceSpike.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AIceSpike::AIceSpike()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;
	
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComponent);
	CollisionComp->InitCapsuleSize(50.f, 50.f);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIceSpike::OnOverlapBegin);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InitialLifeSpan = LifeTime;

	PrimaryActorTick.bCanEverTick = true;
}

void AIceSpike::BeginPlay()
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

void AIceSpike::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OverlappedComp || !OtherActor) return;
	
	APawn* OwnerPawn = Cast<APawn>(GetInstigator());
	if (!OwnerPawn || OtherActor == OwnerPawn) return;
	
	OverlappedComp->SetGenerateOverlapEvents(false);

	if (ACharacter* Char = Cast<ACharacter>(OtherActor))
	{
		const FVector Dir = (Char->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		const FVector LaunchVel = Dir * PushHorizontalStrength + FVector(0, 0, PushUpStrength);
		Char->LaunchCharacter(LaunchVel, true, true);
	}

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		OwnerPawn->GetController(),
		this,
		UDamageType::StaticClass()
	);
}


