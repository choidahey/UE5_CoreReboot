#include "MonsterAI/Skills/SphereDamageSkillActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "CR4S.h"

ASphereDamageSkillActor::ASphereDamageSkillActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	CollisionComp->SetupAttachment(RootComponent);
}

void ASphereDamageSkillActor::BeginPlay()
{
	Super::BeginPlay();

	SetupCollisionComponent();
	SnapToGroundIfNeeded();
	ActivateEffect();
	ApplyInitialOverlapDamage();

	SetLifeSpan(LifeTime);
}

void ASphereDamageSkillActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;

	if (bAffectOnlyGroundTargets && !IsActorOnGround(OtherActor))
	{
		UE_LOG(LogMonster, Log, TEXT("[SphereDamage] Ignored airborne actor: %s"), *OtherActor->GetName());
		return;
	}

	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ASphereDamageSkillActor::SetupCollisionComponent()
{
	if (USphereComponent* Sphere = Cast<USphereComponent>(CollisionComp))
	{
		Sphere->SetSphereRadius(SphereRadius);
		Sphere->SetCollisionProfileName(TEXT("MonsterSkillActor"));
		Sphere->SetGenerateOverlapEvents(true);

		Sphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnOverlap);
	}

//#if WITH_EDITOR
//	DrawDebugSphere(
//		GetWorld(),
//		CollisionComp->GetComponentLocation(),
//		SphereRadius,
//		16,
//		FColor::Red,
//		false,
//		2.0f
//	);
//#endif
}

void ASphereDamageSkillActor::SnapToGroundIfNeeded()
{
	if (!bSnapToGround) return;

	FHitResult HitResult;
	const FVector Start = GetActorLocation() + FVector(0, 0, GroundOffsetZ);
	const FVector End = GetActorLocation() - FVector(0, 0, GroundTraceDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (AActor* InstigatorActor = GetInstigator())
	{
		Params.AddIgnoredActor(InstigatorActor);
	}

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		SetActorLocation(HitResult.ImpactPoint);
	}
}

void ASphereDamageSkillActor::ActivateEffect()
{
	if (IsValid(NiagaraComp))
	{
		NiagaraComp->Activate(true);
	}
}

void ASphereDamageSkillActor::ApplyInitialOverlapDamage()
{
	if (!IsValid(CollisionComp)) return;

	TArray<AActor*> OverlappingActors;
	CollisionComp->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlapActor : OverlappingActors)
	{
		if (!IsValid(OverlapActor)) continue;

		if (bAffectOnlyGroundTargets && !IsActorOnGround(OverlapActor))
		{
			UE_LOG(LogMonster, Log, TEXT("[SphereDamage] Ignored airborne actor (initial): %s"), *OverlapActor->GetName());
			continue;
		}

		ApplyEffectToActor(OverlapActor);
	}
}

bool ASphereDamageSkillActor::IsActorOnGround(AActor* Actor) const
{
	if (const ACharacter* Character = Cast<ACharacter>(Actor))
	{
		return Character->GetCharacterMovement()->IsMovingOnGround();
	}

	return true;
}
