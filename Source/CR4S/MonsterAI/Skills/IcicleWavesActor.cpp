#include "MonsterAI/Skills/IcicleWavesActor.h"
#include "MonsterAI/Skills/Icicle.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

void AIcicleWavesActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIcicleWavesActor::SpawnIcicleWaves(int32 SpawnCount, float BaseDistance, float DistanceStep, float Interval, bool bForwardIn)
{
	ACharacter* BossChar = Cast<ACharacter>(GetOwner());
	if (BossChar)
	{
		FVector BossLoc = BossChar->GetActorLocation();

		float HalfHeight = 0.f;
		if (UCapsuleComponent* Cap = BossChar->FindComponentByClass<UCapsuleComponent>())
		{
			HalfHeight = Cap->GetScaledCapsuleHalfHeight();
		}
		else
		{
			FVector Origin, Extents;
			BossChar->GetActorBounds(false, Origin, Extents);
			HalfHeight = Extents.Z;
		}

		SpawnCenterLocation = FVector(BossLoc.X, BossLoc.Y, BossLoc.Z - HalfHeight);
	}

	SpawnInterval = Interval;
	bForward = bForwardIn;
	SpawnIndex = 0;
	SpawnRadius.Empty();

	for (int32 i = 0; i < SpawnCount; i++)
		SpawnRadius.Add(BaseDistance + DistanceStep * i);

	if (!bForward)
		Algo::Reverse(SpawnRadius);

	SpawnNext();
}

void AIcicleWavesActor::SpawnNext()
{
	if (SpawnIndex >= SpawnRadius.Num())
	{
		Destroy();
		return;
	}

	APawn* BossPawn = Cast<APawn>(GetOwner());
	float Dist = SpawnRadius[SpawnIndex++];

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = BossPawn;

	if (AIcicle* IcicleActor = GetWorld()->SpawnActor<AIcicle>(
		IcicleActorClass,
		SpawnCenterLocation,
		FRotator::ZeroRotator,
		SpawnParams))
	{
		IcicleActor->InitIcicle(Dist);
	}

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AIcicleWavesActor::SpawnNext,
		SpawnInterval,
		false);
}
