#include "MonsterAI/Skills/BreathActor.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ABreathActor::ABreathActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BreathEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BreathEffect"));
	BreathEffect->SetupAttachment(RootComp);
	BreathEffect->SetAutoActivate(false);
}

void ABreathActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreathActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated) return;

	if (FollowMesh && FollowSocketName != NAME_None)
	{
		LastSocketTransform = FollowMesh->GetSocketTransform(FollowSocketName, RTS_World);
		SetActorLocation(LastSocketTransform.GetLocation());
		SetActorRotation(LastSocketTransform.Rotator());
	}

	TimeSinceLastTick += DeltaTime;
	if (TimeSinceLastTick >= TickInterval)
	{
		TimeSinceLastTick = 0.f;
		DoDamage();
	}
}

void ABreathActor::InitializeSkill(USkeletalMeshComponent* InMesh, FName InSocketName)
{
	if (bIsInitialized) return;

	FollowMesh = InMesh;
	FollowSocketName = InSocketName;

	if (AActor* SkillOwner = GetOwner())
	{
		UMonsterSkillComponent* SkillComponent = SkillOwner->FindComponentByClass<UMonsterSkillComponent>();
		if (SkillComponent)
		{
			const FMonsterSkillData& Data = SkillComponent->GetCurrentSkillData();
			Damage = Data.Damage;
			bAllowMultipleHits = Data.bAllowMultipleHits;
		}
	}

	if (StaticMesh)
		StaticMesh->SetVisibility(true);
	if (BreathEffect)
		BreathEffect->DeactivateImmediate();

	bIsInitialized = true;
}

void ABreathActor::ActivateSkill()
{
	if (bIsActivated) return;

	if (StaticMesh)
		StaticMesh->SetVisibility(false);

	if (BreathEffect)
		BreathEffect->Activate(true);

	bIsActivated = true;
	AlreadyDamaged.Empty();
}

void ABreathActor::EndSkill()
{
	Destroy();
}

void ABreathActor::DoDamage()
{
	if (!FollowMesh || FollowSocketName == NAME_None) return;

	const FVector Origin = LastSocketTransform.GetLocation();
	const FVector Forward = LastSocketTransform.GetUnitAxis(EAxis::Z);

	TArray<AActor*> HitActors;
	UGameplayStatics::GetAllActorsOfClass(this, APawn::StaticClass(), HitActors);

	for (AActor* Target : HitActors)
	{
		if (!Target || AlreadyDamaged.Contains(Target)) continue;
		if (Target == GetOwner()) continue;

		const FVector ToTarget = Target->GetActorLocation() - Origin;
		if (ToTarget.Size() > Range) continue;

		const float Dot = FVector::DotProduct(Forward, ToTarget.GetSafeNormal());
		const float CosAngle = FMath::Cos(FMath::DegreesToRadians(Angle / 2.f));
		if (Dot < CosAngle) continue;

		UGameplayStatics::ApplyDamage(
			Target,
			Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

		if (!bAllowMultipleHits)
		{
			AlreadyDamaged.Add(Target);
		}

		UE_LOG(LogTemp, Log, TEXT("[BreathDamage] Hit %s"), *Target->GetName());
	}

#if WITH_EDITOR
	DrawDebugCone(
		GetWorld(),
		Origin,
		Forward,
		Range,
		FMath::DegreesToRadians(Angle / 2.f),
		FMath::DegreesToRadians(Angle / 2.f),
		16,
		FColor::Red,
		false,
		0.1f
	);
#endif
}
