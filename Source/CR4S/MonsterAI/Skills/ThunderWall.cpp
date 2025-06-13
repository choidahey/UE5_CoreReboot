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

	if (UBoxComponent* BoxComp = Cast<UBoxComponent>(CollisionComp))
	{
		BoxComp->SetBoxExtent(WallExtent);
	}
	
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

