#include "ThunderWall.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AThunderWall::AThunderWall()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComponent);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AThunderWall::OnOverlap);
	
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->SetAutoActivate(false);

	SetActorRotation(FRotator::ZeroRotator);
}

void AThunderWall::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraComp)
	{
		NiagaraComp->Activate(true);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		BossActor = OwnerActor;
	}

	if (IsValid(BossActor))
	{
		TargetRotation = BossActor->GetActorRotation();
		SetActorRotation(TargetRotation);
	}
}

void AThunderWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (BossActor)
	{
		FRotator CurrentBossRotation = BossActor->GetActorRotation();
		
		if (!CurrentBossRotation.Equals(TargetRotation, 1.0f))
		{
			TargetRotation = CurrentBossRotation;
		}
		
		if (!GetActorRotation().Equals(TargetRotation, 0.1f))
		{
			SetActorRotation(TargetRotation);
		}
	}
}

void AThunderWall::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

