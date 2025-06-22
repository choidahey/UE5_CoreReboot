#include "FieldActor.h"
#include "CR4S.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"

AFieldActor::AFieldActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>("CollisionComp");
	CollisionComp->SetupAttachment(RootComp);
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AFieldActor::OnOverlap);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AFieldActor::OnEndOverlap);
	
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->bAutoActivate = false;
}

void AFieldActor::BeginPlay()
{
	Super::BeginPlay();
	
	InitialActorScale = GetActorScale3D();
	StartTimers();
	
	CR4S_Log(LogDa, Warning, TEXT("[%s] BeginPlay - Damage : %f"), *MyHeader, Damage);
}

void AFieldActor::Initialize(AActor* OwnerMonster, AActor* Target)
{
	OwnerActor  = OwnerMonster;
	EnemyActor = Target;
	TargetActor = GetTargetActor();

	if (TargetActor)
	{
		InitialOffset = GetActorLocation() - TargetActor->GetActorLocation();
	}
	
	NiagaraComp->Activate(true);
	
	if (bIsAttachGround)
		AttachGround();
	
	const bool bNeedTick = bIsFollowTarget || bDynamicSize;
	PrimaryActorTick.SetTickFunctionEnable(bNeedTick);
}

void AFieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDynamicSize && DynamicScaleDuration > 0.f)
		UpdateDynamicScale(DeltaTime);
	
	if (bIsFollowTarget)
		FollowTarget();
}

void AFieldActor::UpdateDynamicScale(float DeltaTime)
{
	ElapsedTime = FMath::Min(ElapsedTime + DeltaTime, DynamicScaleDuration);
	const float Alpha = ElapsedTime / DynamicScaleDuration;

	FVector CalculateScale;
	if (DynamicScaleCurve)
		CalculateScale = DynamicScaleCurve->GetVectorValue(Alpha);
	else
		CalculateScale = FMath::Lerp(InitialActorScale, InitialActorScale + DynamicScaleOffset, Alpha);

	SetActorScale3D(CalculateScale);
	
	if (bDrawLine)
	{
		DrawDebugCapsule(
			GetWorld(),
			CollisionComp->GetComponentLocation(),
			Cast<UCapsuleComponent>(CollisionComp)->GetScaledCapsuleHalfHeight(),
			Cast<UCapsuleComponent>(CollisionComp)->GetScaledCapsuleRadius(),
			CollisionComp->GetComponentQuat(),
			FColor::Blue,
			false,
			LifeTime,
			0,
			2.f
		);
	}
}

void AFieldActor::AttachGround()
{
	if (!TargetActor) return;

	const FVector BaseLocation = TargetActor->GetActorLocation();
	const FVector GroundLocation = GetGroundLocation(BaseLocation);
	
	SetActorLocation(GroundLocation);
}

void AFieldActor::StartTimers()
{
    GetWorld()->GetTimerManager().SetTimer(
        LifeTimerHandle, this, &AFieldActor::EndSkill, LifeTime, false);

    GetWorld()->GetTimerManager().SetTimer(
        DamageTimerHandle, this, &AFieldActor::ApplyDamageTick,
        DamageTickInterval, true, DamageTickInterval);
}

void AFieldActor::FollowTarget()
{
	if (!TargetActor) return;

	FVector TargetLocation = TargetActor->GetActorLocation() + InitialOffset;

	if (bIsAttachGround)
	{
		FVector GroundLocation = GetGroundLocation(TargetLocation);
		TargetLocation.Z = GroundLocation.Z;
	}
	
	SetActorLocation(TargetLocation);

	if (bDrawLine)
	{
		if (UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(CollisionComp))
		{
			DrawDebugCapsule(
				GetWorld(),
				CollisionComp->GetComponentLocation(),
				CapsuleComp->GetScaledCapsuleHalfHeight(),
				CapsuleComp->GetScaledCapsuleRadius(),
				CollisionComp->GetComponentQuat(),
				FColor::Red,
				false,
				LifeTime,
				0,
				2.f
			);
		}
	}
}

void AFieldActor::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != OwnerActor && !OtherActor->IsA(ABaseMonster::StaticClass()))
		OverlappingActors.Add(OtherActor);

	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AFieldActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
    OverlappingActors.Remove(OtherActor);
}

void AFieldActor::ApplyDamageTick()
{
	if (OverlappingActors.Num() == 0) return;
    
    TSet<AActor*> ActorsCopy = OverlappingActors;
        
	for (AActor* Actor : ActorsCopy)
	{
		if (!IsValid(Actor) || Cast<ABaseMonster>(Actor) || Cast<AAnimalMonster>(Actor))
		{
			OverlappingActors.Remove(Actor);
			continue;
		}
		
		UGameplayStatics::ApplyDamage(Actor, Damage, GetInstigatorController(), this, nullptr);
	}
}

void AFieldActor::EndSkill()
{
	GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
	Destroy();
}

FVector AFieldActor::GetGroundLocation(const FVector& BaseLocation) const
{
	const FVector Start = BaseLocation + FVector(0, 0, TraceHeight);
	const FVector End = BaseLocation - FVector(0, 0, TraceHeight);
	
	FCollisionQueryParams Params(NAME_None, false, this);
	if (TargetActor)
		Params.AddIgnoredActor(TargetActor);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		return Hit.Location;
	
	return BaseLocation;
}

AActor* AFieldActor::GetTargetActor() const
{
	AActor* InTarget;
	switch (SpawnTarget)
	{
		case ESpawnSource::Owner:
			InTarget = OwnerActor;
			return InTarget;
		case ESpawnSource::Enemy:
			InTarget = EnemyActor;
			return InTarget;
		default:
			return nullptr;
	}
}