#include "MonsterAI/Skills/MudField.h"
#include "Kismet/KismetMathLibrary.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Skills/RotatingProjectile.h"

AMudField::AMudField()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp");
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
	if (!MudActorClass) return;
	
	FActorSpawnParameters Params;
	Params.Owner = Cast<AActor>(GetOwner());
	Params.Instigator = GetInstigator();
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	FVector Origin = GetActorLocation();
	FRotator SpawnRot = FRotator::ZeroRotator;
	
	ARotatingProjectile* Proj = GetWorld()->SpawnActor<ARotatingProjectile>(MudActorClass, Origin, SpawnRot, Params);
	if (!Proj) return;
	
	Proj->SetBossActor(GetOwner(), NAME_None);
	
	FVector2D Rand2D = FMath::RandPointInCircle(300.f);
	float RandZ = FMath::FRandRange(MinSpawnZOffset, MaxSpawnZOffset);
	FVector TargetLoc = Origin + FVector(Rand2D.X, Rand2D.Y, RandZ);
	
	float LaunchSpeed = 1000.f;
	Proj->LaunchProjectile(TargetLoc, LaunchSpeed);
}

void AMudField::SetupNiagara() const
{
	if (NiagaraComp->GetAsset())
	{
		NiagaraComp->Activate(true);
	}
}
