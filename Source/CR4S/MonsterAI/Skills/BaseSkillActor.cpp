#include "MonsterAI/Skills/BaseSkillActor.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "Utility/CombatStatics.h"
#include "CR4S.h"

ABaseSkillActor::ABaseSkillActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->SetAutoActivate(false);
}

void ABaseSkillActor::BeginPlay()
{
	Super::BeginPlay();
	InitializeSkillData();

	PlaySkillSound(LaunchSkillSound);

}

void ABaseSkillActor::PlaySkillSound(USoundBase* Sound)
{
	if (!Sound) return;

	if (UAudioManager* AudioMgr = GetWorld()->GetGameInstance()->GetSubsystem<UAudioManager>())
	{
		AudioMgr->PlaySFX(Sound, GetActorLocation(), StartSoundType);
	}
}

void ABaseSkillActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_WorldStatic) return;
	ApplyEffectToActor(OtherActor);
}

void ABaseSkillActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_WorldStatic) return;
	ApplyEffectToActor(OtherActor);
}

void ABaseSkillActor::InitializeSkillData()
{
	if (AActor* SkillOwner = GetOwner())
	{
		if (UMonsterSkillComponent* SkillComp = SkillOwner->FindComponentByClass<UMonsterSkillComponent>())
		{
			const FMonsterSkillData& Data = SkillComp->GetCurrentSkillData();
			Damage = Data.Damage;
			StunGaugeAmount = Data.StunGauge;
			bAllowMultipleHits = Data.bAllowMultipleHits;
		}
	}
}

void ABaseSkillActor::ApplyInitialOverlapDamage()
{
	if (!IsValid(CollisionComp)) return;

	TArray<AActor*> OverlappingActors;
	CollisionComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		ApplyEffectToActor(Actor);
	}
}

void ABaseSkillActor::ApplyEffectToActor(AActor* Target)
{
	if (!IsValid(Target) || Target == GetOwner() || Target == GetInstigator()) return;
	if (Cast<ABaseMonster>(Target) || Cast<AAnimalMonster>(Target) || Cast<ABaseSkillActor>(Target)) return;
	if (!bAllowMultipleHits && AlreadyDamaged.Contains(Target)) return;

	if (UMonsterStateComponent* StateComp = Target->FindComponentByClass<UMonsterStateComponent>())
		bIsStunned = StateComp->IsStunned();

	if (UMonsterAttributeComponent* AttrComp = Target->FindComponentByClass<UMonsterAttributeComponent>())
	{
		const FMonsterAttributeRow& AttrData = AttrComp->GetMonsterAttribute();
		StunDamageMultiplier = AttrData.StunDamageMultiplier;
	}
	
	if (Damage > 0.f)
	{
		if (bIsStunned && StunDamageMultiplier > 0.f)
			Damage *= StunDamageMultiplier;

		UGameplayStatics::ApplyDamage(Target, Damage, GetInstigatorController(), this, UDamageType::StaticClass());

#if WITH_EDITOR
		const FString AttackerName = GetOwner() ? GetOwner()->GetName() : TEXT("UnknownOwner");
		const FString SkillName = GetName();
		const FString VictimName = Target->GetName();

		UE_LOG(LogMonster, Log, TEXT("[SkillHit] %s used %s on %s �� Damage: %.1f, Stun: %.1f"),
			*AttackerName,
			*SkillName,
			*VictimName,
			Damage,
			StunGaugeAmount
		);
#endif

	}

	if (Target->GetClass()->ImplementsInterface(UStunnableInterface::StaticClass()))
		UCombatStatics::ApplyStun(Target, StunGaugeAmount);

	AlreadyDamaged.Add(Target);

	PlayEffectAtLocation(Target->GetActorLocation());
	PlaySoundAtLocation(Target->GetActorLocation());
}

void ABaseSkillActor::PlayEffectAtLocation(const FVector& Location)
{
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, Location);
	}
}

void ABaseSkillActor::PlaySoundAtLocation(const FVector& Location)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, Location);
	}
}
