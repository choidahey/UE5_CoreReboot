#include "MonsterAI/Skills/BaseSkillActor.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

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

}

void ABaseSkillActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ApplyEffectToActor(OtherActor);
}

void ABaseSkillActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
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

void ABaseSkillActor::ApplyEffectToActor(AActor* Target)
{
	if (!IsValid(Target) || Target == GetOwner() || Target == GetInstigator()) return;
	if (Cast<ABaseMonster>(Target)) return;
	if (!bAllowMultipleHits && AlreadyDamaged.Contains(Target)) return;

	if (Damage > 0.f)
	{
		UGameplayStatics::ApplyDamage(Target, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	// TODO: 
	// Target->ApplyStuntGauge(StuntGauge);

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

