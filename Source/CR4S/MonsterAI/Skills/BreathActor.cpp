#include "MonsterAI/Skills/BreathActor.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

ABreathActor::ABreathActor()
{
	PrimaryActorTick.bCanEverTick = true;

	if (NiagaraComp)
	{
		NiagaraComp->SetAutoActivate(false);
	}
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

	if (StaticMesh)
		StaticMesh->SetVisibility(true);
	if (NiagaraComp)
		NiagaraComp->DeactivateImmediate();

	bIsInitialized = true;
}

void ABreathActor::ActivateSkill()
{
	if (bIsActivated) return;

	if (StaticMesh)
		StaticMesh->SetVisibility(false);

	if (NiagaraComp)
		NiagaraComp->Activate(true);

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
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), HitActors);

	TSet<APlayerCharacter*> MountedPlayers;
	for (TActorIterator<AModularRobot> It(GetWorld()); It; ++It)
	{
		if (APlayerCharacter* Mounted = It->GetMountedCharacter())
		{
			MountedPlayers.Add(Mounted);
		}
	}

	for (AActor* Target : HitActors)
	{
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(Target))
		{
			if (MountedPlayers.Contains(Player))
			{
				continue;
			}
		}

		const FVector ToTarget = Target->GetActorLocation() - Origin;
		if (ToTarget.Size() > Range) continue;

		const float Dot = FVector::DotProduct(Forward, ToTarget.GetSafeNormal());
		const float CosAngle = FMath::Cos(FMath::DegreesToRadians(Angle / 2.f));
		if (Dot < CosAngle) continue;

		ApplyEffectToActor(Target);
	}

//#if WITH_EDITOR
//	DrawDebugCone(
//		GetWorld(),
//		Origin,
//		Forward,
//		Range,
//		FMath::DegreesToRadians(Angle / 2.f),
//		FMath::DegreesToRadians(Angle / 2.f),
//		16,
//		FColor::Red,
//		false,
//		0.1f
//	);
//#endif
}
