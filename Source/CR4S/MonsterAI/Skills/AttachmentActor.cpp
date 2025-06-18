#include "MonsterAI/Skills/AttachmentActor.h"
#include "MonsterAI/Region/AlgorVexSnowBoss.h"
#include "Components/BoxComponent.h"

AAttachmentActor::AAttachmentActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAttachmentActor::OnOverlap);
}

void AAttachmentActor::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	ElapsedTime = 0.f;

	if (AAlgorVexSnowBoss* AlgorVex = Cast<AAlgorVexSnowBoss>(GetOwner()))
	{
		AlgorVex->bIsIceClawBuffActive = true;
		UE_LOG(LogTemp, Log, TEXT("[AlgorVex] IceClawBuff applied for %.1f sec"), Duration);
	}
}

void AAttachmentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	if (ElapsedTime >= Duration || (bIsDestructible && CurrentHealth <= 0.f))
	{
		DeactivateBuffAndDestroy();
	}
}

float AAttachmentActor::TakeDamage(float DamageAmount, FDamageEvent const&, AController*, AActor*)
{
	if (!bIsDestructible) return 0.f;

	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.f)
	{
		DeactivateBuffAndDestroy();
	}
	return DamageAmount;
}

void AAttachmentActor::DeactivateBuffAndDestroy()
{
	if (AAlgorVexSnowBoss* AlgorVex = Cast<AAlgorVexSnowBoss>(GetOwner()))
	{
		if (AlgorVex->bIsIceClawBuffActive)
		{
			AlgorVex->bIsIceClawBuffActive = false;
			UE_LOG(LogTemp, Log, TEXT("[AlgorVex] IceClawBuff deactivated."));
		}
	}

	Destroy();
}
