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

// NOTICE :: Not Use
ABlizzard::ABlizzard()
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

void ABlizzard::Launch(APawn* BossOwnerPawn)
{
	if (!BossOwnerPawn) return;

	OwnerMonster = Cast<ABaseMonster>(BossOwnerPawn);
	
	const UMonsterAttributeComponent* AttrComp = OwnerMonster->FindComponentByClass<UMonsterAttributeComponent>();
	const UMonsterSkillComponent* SkillComp = OwnerMonster->FindComponentByClass<UMonsterSkillComponent>();
	if (!AttrComp || !SkillComp) return;

	Speed = AttrComp->GetMonsterAttribute().MoveSpeed;
	Damage = SkillComp->GetCurrentSkillData().Damage;
	
	if (UCharacterMovementComponent* MovementComp = OwnerMonster->GetCharacterMovement())
	{
		Speed = MovementComp->MaxWalkSpeed;
		MovementComp->MaxWalkSpeed = Speed * SpeedBuff;
	}
	
	OverlapComp->SetCapsuleSize(SpawnRadius, SpawnHeight * 0.5f);

	// TODO :: I Will Refectoring
	/*FVector GetNiagaraScale= NiagaraComp->GetRelativeScale3D();
	NiagaraComp->SetWorldScale3D(
		FVector(SpawnRadius / GetNiagaraScale.X, SpawnRadius / GetNiagaraScale.Y, SpawnHeight / GetNiagaraScale.Z * 0.5f)
		);
	UE_LOG(LogTemp, Display, TEXT("Niagara Scale : %f, %f, %f"), GetNiagaraScale.X, GetNiagaraScale.Y, GetNiagaraScale.Z);
	
	NiagaraComp->SetWorldScale3D(
		FVector(SpawnRadius / 500.f, SpawnRadius / 500.f, SpawnHeight / 500.f * 0.5f)
		);*/

	float BossHalfHeight = 0.f;
	if (auto* BossCapsule = OwnerMonster->FindComponentByClass<UCapsuleComponent>())
	{
		BossHalfHeight = BossCapsule->GetScaledCapsuleHalfHeight();
	}
	
	NiagaraComp->Activate(true);

	AttachToActor(OwnerMonster, FAttachmentTransformRules::KeepRelativeTransform);
	SetActorRelativeLocation(FVector(0, 0, -BossHalfHeight));
	OverlapComp->SetRelativeLocation(FVector(0.f, 0.f, BossHalfHeight));

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
	// NOTICE :: Test Log
	const FVector DebugCenter  = OverlapComp->GetComponentLocation();
	const float   DebugHalfH   = OverlapComp->GetScaledCapsuleHalfHeight();
	const float   DebugRadius  = OverlapComp->GetScaledCapsuleRadius();
	const FQuat   DebugQuat    = OverlapComp->GetComponentQuat();
	DrawDebugCapsule(
		GetWorld(),
		DebugCenter,
		DebugHalfH,
		DebugRadius,
		DebugQuat,
		FColor::Red,
		false,
		LifeTime,
		0,
		2.0f
	);
	
	TArray<AActor*> OverlapActors;
	OverlapComp->GetOverlappingActors(OverlapActors);

	UE_LOG(LogTemp, Log, TEXT("[%s] Blizzard overlaps: %d actors"), *MyHeader, OverlapActors.Num());
	
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



