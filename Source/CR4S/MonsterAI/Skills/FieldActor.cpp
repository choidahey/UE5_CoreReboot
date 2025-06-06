#include "FieldActor.h"
#include "CR4S.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AFieldActor::AFieldActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("CollisionComp");
	CapsuleComp->SetupAttachment(RootComponent);
	CapsuleComp->InitCapsuleSize(InitCapsuleRadius, InitCapsuleHalfHeight);
	
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CapsuleComp->SetGenerateOverlapEvents(true);
	
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AFieldActor::OnBeginOverlap);
	CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AFieldActor::OnEndOverlap);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComp");
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->bAutoActivate = false;
}

void AFieldActor::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerMonster = GetOwner())
	{
		if (UMonsterSkillComponent* SkillComp = OwnerMonster->FindComponentByClass<UMonsterSkillComponent>())
		{
			const FMonsterSkillData& SkillData = SkillComp->GetCurrentSkillData();
			Damage = SkillData.Damage;
		}
	}

	CR4S_Log(LogDa, Warning, TEXT("[%s] BeginPlay - Damage : %f"), *MyHeader, Damage);
}

void AFieldActor::Initialize(AActor* OwnerMonster, AActor* Target)
{
	OwnerActor  = OwnerMonster;
	TargetActor = Target;

	NiagaraComp->Activate(true);

	if (!bDynamicSize)
		ApplyInitialScale();
	
	PerformGroundTrace();
	StartTimers();
	
	const bool bNeedTick = bFollowOwner || bDynamicSize;
	PrimaryActorTick.SetTickFunctionEnable(bNeedTick);
}

void AFieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDynamicSize && DynamicScaleDuration > 0.f)
		UpdateDynamicScale(DeltaTime);

	if (bFollowOwner && OwnerActor)
		FollowOwner();
}

void AFieldActor::ApplyInitialScale()
{
	SetActorScale3D(InitialNiagaraScale);
	NiagaraComp->SetWorldScale3D(InitialNiagaraScale);

	// TODO :: I will fix capsule collision size(why cloudy is bad size?)
	// CapsuleComp->SetAbsolute(true, true, true);
	CapsuleComp->SetAbsolute(false, false, false);
	const FBoxSphereBounds NiagaraBounds = NiagaraComp->CalcBounds(NiagaraComp->GetComponentTransform());
	const float CapsuleRadius = NiagaraBounds.BoxExtent.X * InitCapsuleRadius;
	const float CapsuleHalfHeight = NiagaraBounds.BoxExtent.Z * InitCapsuleHalfHeight;
	CapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);

#if WITH_EDITOR
	CR4S_Log(LogDa, Warning, TEXT(
		"[%s] NiagaraBounds=(%.2f,%.2f,%.2f)" "→ CapsuleRadius=%.2f, CapsuleHalfHeight=%.2f")
		, *MyHeader
		, NiagaraBounds.BoxExtent.X
		, NiagaraBounds.BoxExtent.Y
		, NiagaraBounds.BoxExtent.Z
		, CapsuleRadius
		, CapsuleHalfHeight
	);
	
	DrawDebugCapsule(
		GetWorld(),
		CapsuleComp->GetComponentLocation(),
		CapsuleComp->GetScaledCapsuleHalfHeight(),
		CapsuleComp->GetScaledCapsuleRadius(),
		CapsuleComp->GetComponentQuat(),
		FColor::Cyan,
		false,
		LifeTime,
		0,
		2.f
	);
#endif
}

void AFieldActor::PerformGroundTrace()
{
	const FVector Base = (SpawnSource == ESpawnSource::Target && TargetActor)
		? TargetActor->GetActorLocation()
		: (OwnerActor ? OwnerActor->GetActorLocation() : GetActorLocation());

	const FVector Start = Base + FVector(0,0,TraceHeight);
	const FVector End = Base - FVector(0,0,TraceHeight);

#if WITH_EDITOR
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor::Yellow,
		false,
		LifeTime,
		0,
		2.f
	);
#endif

	FHitResult Hit;
	FCollisionQueryParams Params(NAME_None, false, OwnerActor);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		SetActorLocation(Hit.Location);
		NiagaraComp->SetWorldLocation(Hit.Location);
	}
}

void AFieldActor::StartTimers()
{
    GetWorld()->GetTimerManager().SetTimer(
        LifeTimerHandle, this, &AFieldActor::EndSkill, LifeTime, false);

    GetWorld()->GetTimerManager().SetTimer(
        DamageTimerHandle, this, &AFieldActor::ApplyDamageTick,
        DamageTickInterval, true, DamageTickInterval);
}

void AFieldActor::UpdateDynamicScale(float DeltaTime)
{
	ElapsedTime = FMath::Min(ElapsedTime + DeltaTime, DynamicScaleDuration);
	const float Alpha = ElapsedTime / DynamicScaleDuration;

	FVector CalculatedNiagaraScale;
	if (DynamicScaleCurve)
		CalculatedNiagaraScale = DynamicScaleCurve->GetVectorValue(ElapsedTime);
	else
		CalculatedNiagaraScale = FMath::Lerp(InitialNiagaraScale, InitialNiagaraScale + DynamicScaleOffset, Alpha);
	
	NiagaraComp->SetWorldScale3D(CalculatedNiagaraScale);
	SetActorScale3D(CalculatedNiagaraScale);
	
	CapsuleComp->SetAbsolute(true, true, true);
	const FBoxSphereBounds NiagaraBounds = NiagaraComp->CalcBounds(NiagaraComp->GetComponentTransform());
	const float NewRadius = NiagaraBounds.BoxExtent.X / InitialNiagaraScale.X;
	const float NewHalfHeight = NiagaraBounds.BoxExtent.Z / InitialNiagaraScale.X;
	CapsuleComp->SetCapsuleSize(NewRadius, NewHalfHeight);

#if WITH_EDITOR
	CR4S_Log(LogDa, Log, TEXT(
			"[%s] Dynamic NiagaraBounds=(%.2f,%.2f,%.2f) →  Radius=%f, HalfHeight=%f"
			),
			*MyHeader,
			NiagaraBounds.BoxExtent.X,
			NiagaraBounds.BoxExtent.Y,
			NiagaraBounds.BoxExtent.Z,
			NewRadius,
			NewHalfHeight
		);
#endif
}

void AFieldActor::FollowOwner()
{
    const FVector Base = OwnerActor->GetActorLocation();
    const FVector Start = Base + FVector(0,0,TraceHeight);
    const FVector End   = Base - FVector(0,0,TraceHeight);

    FHitResult Hit;
    FCollisionQueryParams Params(NAME_None, false, OwnerActor);
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        SetActorLocation(Hit.Location);
    	NiagaraComp->SetWorldLocation(Hit.Location);

#if WITH_EDITOR
    	DrawDebugCapsule(
			GetWorld(),
			CapsuleComp->GetComponentLocation(),
			CapsuleComp->GetScaledCapsuleHalfHeight(),
			CapsuleComp->GetScaledCapsuleRadius(),
			CapsuleComp->GetComponentQuat(),
			FColor::Blue,
			false,
			LifeTime,
			0,
			2.f
		);
#endif
    }
}

void AFieldActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
    int32 BodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != OwnerActor && !OtherActor->IsA(ABaseMonster::StaticClass()))
	{
		OverlappingActors.Add(OtherActor);
	}
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
		if (!IsValid(Actor))
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
