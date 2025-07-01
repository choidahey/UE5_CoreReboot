#include "MonsterAI/Skills/MudField.h"
#include "Kismet/KismetMathLibrary.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "NiagaraComponent.h"
#include "ProjectileBomb.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Skills/RotatingProjectile.h"

AMudField::AMudField()
{
	PrimaryActorTick.bCanEverTick = true;

	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->bAutoActivate = false;
}

void AMudField::BeginPlay()
{
	Super::BeginPlay();
	
	SetupNiagara();
	
	if (MudActorClass && SpawnInterval > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AMudField::SpawnMud,
			SpawnInterval,
			true,
			0.0f
		);
	}
}

void AMudField::SpawnMud()
{
	if (!MudActorClass || SpawnCount <= 0) return;
	
	FActorSpawnParameters Params;
	Params.Owner = Cast<AActor>(GetOwner());
	Params.Instigator = GetInstigator();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	const float RandomZOffset = FMath::FRandRange(MinSpawnZOffset, MaxSpawnZOffset);
	const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, RandomZOffset);
	const FRotator SpawnRot = FRotator::ZeroRotator;

	if (SpawnCount > 0)
	{
		for (int32 i = 0; i < SpawnCount; ++i)
		{
			AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(MudActorClass, SpawnLocation, SpawnRot, Params);
			if (!SpawnActor) continue;
			
			if (AProjectileBomb* Bomb = Cast<AProjectileBomb>(SpawnActor))
			{
				Bomb->SetOwner(GetOwner());
				// Bomb->LaunchProjectile();
			}
			else if (ARotatingProjectile* Proj = Cast<ARotatingProjectile>(SpawnActor))
			{
				constexpr float Speed = 1.f;
				Proj->SetBossActor(GetOwner(), NAME_None);
				
				const FVector2D Rand2D = FMath::RandPointInCircle(300.f);
				const FVector TargetLoc = GetActorLocation() + FVector(Rand2D.X, Rand2D.Y, RandomZOffset);
				
				Proj->LaunchProjectile(TargetLoc, Speed);
			}
		}
	}
}

void AMudField::SetupNiagara() const
{
	if (NiagaraComp->GetAsset())
	{
		NiagaraComp->Activate(true);
	}
}
