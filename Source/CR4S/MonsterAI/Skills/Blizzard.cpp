#include "Blizzard.h"

#include "MovieSceneTracksComponentTypes.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/BaseMonster.h"

ABlizzard::ABlizzard()
	: MyHeader(TEXT("Blizzard"))
{
	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	SetRootComponent(RootComp);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp");
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->bAutoActivate = false;

	OverlapComp = CreateDefaultSubobject<UCapsuleComponent>("OverlapComp");
	OverlapComp->SetupAttachment(RootComp);
	
	OverlapComp->InitCapsuleSize(SpawnRadius, SpawnHeight * 0.5f);
	
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	OverlapComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	OverlapComp->SetGenerateOverlapEvents(true);
}

void ABlizzard::BeginPlay()
{
	Super::BeginPlay();
}

void ABlizzard::Launch(ABaseMonster* BossMonster)
{
	if (!BossMonster) return;

	OwnerMonster = BossMonster;
	
	const UMonsterAttributeComponent* AttrComp = BossMonster->FindComponentByClass<UMonsterAttributeComponent>();
	const UMonsterSkillComponent* SkillComp = BossMonster->FindComponentByClass<UMonsterSkillComponent>();
	if (!AttrComp || !SkillComp) return;

	Speed = AttrComp->GetMonsterAttribute().MoveSpeed;
	Damage = SkillComp->GetCurrentSkillData().Damage;
	
	if (UCharacterMovementComponent* MovementComp = BossMonster->GetCharacterMovement())
	{
		Speed = MovementComp->MaxWalkSpeed;
		MovementComp->MaxWalkSpeed = Speed * SpeedBuff;
	}
	
	OverlapComp->SetCapsuleSize(SpawnRadius, SpawnHeight * 0.5f);
	NiagaraComp->SetWorldScale3D(
		FVector(SpawnRadius / 5.f, SpawnRadius / 5.f, SpawnHeight / 5.f * 0.5f)
		);

	float BossHalfHeight = 0.f;
	if (auto* BossCapsule = BossMonster->FindComponentByClass<UCapsuleComponent>())
	{
		BossHalfHeight = BossCapsule->GetScaledCapsuleHalfHeight();
	}
	
	const float ZOffset = -BossHalfHeight + (SpawnHeight * 0.5f);
	
	NiagaraComp->Activate(true);

	AttachToActor(BossMonster, FAttachmentTransformRules::KeepRelativeTransform);
	SetActorRelativeLocation(FVector(0, 0, ZOffset));

	DrawDebugCapsule(
		GetWorld(),
		GetActorLocation(),
		SpawnHeight,
		SpawnRadius,
		GetActorRotation().Quaternion(),
		FColor::Red,
		false,
		LifeTime,
		0,
		2.0f
	);

	GetWorld()->GetTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&ABlizzard::ApplyDamageTick,
		TickInterval,
		true,
		1.f
		);

	GetWorld()->GetTimerManager().SetTimer(
		LifetimeTimerHandle,
		this,
		&ABlizzard::EndSkill,
		LifeTime,
		false);
}

void ABlizzard::ApplyDamageTick()
{
	TArray<AActor*> OverlapActors;
	OverlapComp->GetOverlappingActors(OverlapActors);

	UE_LOG(LogTemp, Log, TEXT("Blizzard overlaps: %d actors"), OverlapActors.Num());
	
	for (AActor* Other : OverlapActors)
	{
		if (!Other || Other == OwnerMonster)
			continue;

		UGameplayStatics::ApplyDamage(
			Other,
			Damage,
			OwnerMonster->GetController(),
			this,
			nullptr);
	}
}

void ABlizzard::EndSkill()
{
	if (OwnerMonster)
	{
		if (UCharacterMovementComponent* MovementComp = OwnerMonster->GetCharacterMovement())
		{
			MovementComp->MaxWalkSpeed = Speed;
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LifetimeTimerHandle);

	Destroy();
}



