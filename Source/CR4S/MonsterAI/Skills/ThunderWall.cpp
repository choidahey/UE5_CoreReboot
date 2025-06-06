#include "ThunderWall.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AThunderWall::AThunderWall()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = Root;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComponent);

	CollisionComp->SetBoxExtent(WallExtent);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AThunderWall::OnOverlapBegin);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
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

	CollisionComp->SetBoxExtent(WallExtent);

	if (AActor* OwnerActor = GetOwner())
	{
		if (UMonsterSkillComponent* SkillComp = OwnerActor->FindComponentByClass<UMonsterSkillComponent>())
		{
			const FMonsterSkillData& SkillData = SkillComp->GetCurrentSkillData();
			Damage = SkillData.Damage;
		}
	}
	
}

void AThunderWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AThunderWall::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
		return;

	APawn* OwnerPawn = Cast<APawn>(GetInstigator());
	if (!OwnerPawn || OtherActor == OwnerPawn) return;
	
	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		OwnerPawn->GetController(),
		this,
		UDamageType::StaticClass()
	);
}

