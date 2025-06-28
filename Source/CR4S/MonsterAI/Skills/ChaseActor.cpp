#include "MonsterAI/Skills/ChaseActor.h"

#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"

AChaseSkillActor::AChaseSkillActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(FName("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AChaseSkillActor::OnOverlap);
	
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->bAutoActivate = false;
}

void AChaseSkillActor::BeginPlay()
{
	Super::BeginPlay();

	if (Lifetime > 0.f)
	{
		SetLifeSpan(Lifetime);
	}

	Target = UGameplayStatics::GetPlayerPawn(this, 0);
	 
	if (DelayTime > KINDA_SMALL_NUMBER)
	{
		GetWorldTimerManager().SetTimer(
			DelayTimerHandle,
			this,
			&AChaseSkillActor::ActivateChase,
			DelayTime,
			false
		);
	}
	else
	{
		ActivateChase();
	}
	
}

void AChaseSkillActor::ActivateChase()
{
	bIsChasing = true;
}

void AChaseSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsChasing || !Target)
		return;

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation  = Target->GetActorLocation();

	if (!bLockZ)
	{
		if (FMath::IsNearlyEqual(CurrentLocation.Z, TargetLocation.Z, KINDA_SMALL_NUMBER))
		{
			bLockZ = true;
		}
	}
	if (bLockZ)
	{
		TargetLocation.Z = CurrentLocation.Z;
	}

	const float DistToTarget = FVector::Dist(CurrentLocation, TargetLocation);
	if (DistToTarget <= SpawnDistance)
	{
		if (SpawnActorClass)
		{
			const FTransform Tf(GetActorRotation(), CurrentLocation);
			
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = Cast<APawn>(GetOwner());

			GetWorld()->SpawnActor<AActor>(SpawnActorClass, Tf, Params);
		}
		
		Destroy();
		return;
	}

	const FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	const FVector NewLoc = CurrentLocation + Direction * ChaseSpeed * DeltaTime;
	SetActorLocation(NewLoc);

	if (bEnableMeshRotation && StaticMesh)
	{
		FVector Axis = RotationAxis.GetSafeNormal();
		float Radians = FMath::DegreesToRadians(RotationSpeed * DeltaTime);
		FQuat DeltaQuat(Axis, Radians);
		StaticMesh->AddLocalRotation(DeltaQuat);
	}

	if (bEnableGroundEffectSpawn && MovementSpawnClass && MovementSpawnInterval > 0.f)
	{
		MovementSpawnTimer += DeltaTime;
		if (MovementSpawnTimer >= MovementSpawnInterval)
		{
			MovementSpawnTimer = 0.f;
			
			FHitResult Hit;
			FVector Start = CurrentLocation;
			FVector End = Start - FVector(0.f, 0.f, 1000.f);

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
			{
				FTransform Tf(FRotator::ZeroRotator, Hit.ImpactPoint);
				GetWorld()->SpawnActor<AActor>(MovementSpawnClass, Tf);
			}
		}
	}
}

void AChaseSkillActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ABaseSkillActor::StaticClass())) return;

	if (OtherActor && OtherActor == GetInstigator()) return;
		
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	Destroy();
}