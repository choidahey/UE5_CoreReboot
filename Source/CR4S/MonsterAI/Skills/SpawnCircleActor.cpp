#include "MonsterAI/Skills/SpawnCircleActor.h"
#include "CR4S.h"
#include "Kismet/KismetMathLibrary.h"


ASpawnCircleActor::ASpawnCircleActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnCircleActor::SpawnInCircle(AActor* InOwnerActor)
{
	OwnerActor = InOwnerActor;
	
	if (!IsValid(OwnerActor) || !SpawnActorClass) return;

	FVector Center = OwnerActor->GetActorLocation();
	
	const int32 Count = FMath::Max(1, SpawnCount);

	for (int32 i = 0; i < Count; ++i)
	{
		float Angle = i * 2 * PI / SpawnCount;
		FVector SpawnLoc = Center + FVector(
			FMath::Cos(Angle) * CircleRadius,
			FMath::Sin(Angle) * CircleRadius,
			0.f
		);
		
		FRotator SpawnRot = UKismetMathLibrary::FindLookAtRotation(Center, SpawnLoc);

		if (bActorFaceCenter)
		{
			SpawnRot = UKismetMathLibrary::FindLookAtRotation(SpawnLoc, Center);
		}

		FActorSpawnParameters Params;
		Params.Owner = OwnerActor;
		Params.Instigator = OwnerActor->GetInstigator();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActor* NewActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, SpawnLoc, SpawnRot, Params);
		if (NewActor && SpawnedLifeTime > 0.f)
		{
			NewActor->SetLifeSpan(SpawnedLifeTime);
		}
	}

	Destroy();
}

void ASpawnCircleActor::BeginPlay()
{
	Super::BeginPlay();
}

