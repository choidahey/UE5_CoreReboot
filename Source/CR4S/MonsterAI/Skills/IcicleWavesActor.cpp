#include "MonsterAI/Skills/IcicleWavesActor.h"
#include "MonsterAI/Skills/Icicle.h"
#include "MonsterAI/Skills/GroundShockActor.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

void AIcicleWavesActor::InitIcicleWaves(int32 SpawnCount, float BaseDistance, float DistanceStep, float Interval, bool bForwardIn)
{
	if (SpawnCount <= 0) return;
	
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

	SpawnIcicleWaves();
}

void AIcicleWavesActor::SpawnIcicleWaves()
{
	if (SpawnIndex >= SpawnRadius.Num())
	{
		Destroy();
		return;
	}

	APawn* BossPawn = Cast<APawn>(GetOwner());
	float Dist = SpawnRadius[SpawnIndex++];

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = BossPawn;
	SpawnParams.Instigator = BossPawn;
	
	if (AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(
		IcicleActorClass,
		SpawnCenterLocation,
		FRotator::ZeroRotator,
		SpawnParams))
	{
		if (AIcicle* Icicle = Cast<AIcicle>(SpawnActor))
			Icicle->InitIcicle(Dist);

		if (AGroundShockActor* GroundShock = Cast<AGroundShockActor>(SpawnActor))
			GroundShock->InitShock(BossPawn->GetActorRotation());
	}
	
	GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AIcicleWavesActor::SpawnIcicleWaves,
			SpawnInterval,
			false
		);
}
