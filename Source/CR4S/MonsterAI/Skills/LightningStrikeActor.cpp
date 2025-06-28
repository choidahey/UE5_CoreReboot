#include "MonsterAI/Skills/LightningStrikeActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"

ALightningStrikeActor::ALightningStrikeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LightningCollider"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ALightningStrikeActor::OnOverlap);
}

void ALightningStrikeActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALightningStrikeActor::InitializeStrike(const FVector& TargetLocation, UNiagaraSystem* LightningEffect)
{
	FVector GroundTarget = TargetLocation;

	FVector Start = GroundTarget + FVector(0.f, 0.f, TraceHeightAbove);
	FVector End = GroundTarget - FVector(0.f, 0.f, TraceDepthBelow);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		GroundTarget.Z = HitResult.ImpactPoint.Z;
	}

	const float CapsuleTopZ = TargetLocation.Z + TraceHeightAbove;
	const float CapsuleBottomZ = GroundTarget.Z;
	const float CapsuleHeight = CapsuleTopZ - CapsuleBottomZ;
	const float CapsuleHalfHeight = CapsuleHeight * 0.5f;
	const float CapsuleCenterZ = CapsuleBottomZ + CapsuleHalfHeight;

	SetActorLocation(GroundTarget);
	InitializeSkillData();

	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(CollisionComp);
	if (Capsule)
	{
		Capsule->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		Capsule->SetRelativeLocation(FVector(0.f, 0.f, CapsuleHalfHeight));
	}

	if (LightningEffect && NiagaraComp)
	{
		NiagaraComp->SetAsset(LightningEffect);
		NiagaraComp->SetWorldScale3D(FVector(10.f));
		NiagaraComp->SetVariableFloat(FName("User_Width"), LightningWidth);
		NiagaraComp->SetWorldLocation(GroundTarget);
		NiagaraComp->Activate(true);
	}

#if WITH_EDITOR
	if (IsValid(CollisionComp))
	{
		DrawDebugCapsule(
			GetWorld(),
			CollisionComp->GetComponentLocation(),
			CapsuleHalfHeight,
			CapsuleRadius,
			CollisionComp->GetComponentQuat(),
			FColor::Red,
			false,
			2.f,
			0,
			2.f
		);
	}
#endif

	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestroyHandle,
		this,
		&ALightningStrikeActor::HandleSelfDestruct,
		LightningDelayBeforeStrike + ActorLifeAfterStrike,
		false
	);
}

void ALightningStrikeActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ALightningStrikeActor::HandleSelfDestruct()
{
	Destroy();
}
