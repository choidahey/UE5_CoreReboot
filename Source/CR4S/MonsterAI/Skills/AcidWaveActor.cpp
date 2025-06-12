#include "MonsterAI/Skills/AcidWaveActor.h"
#include "CR4S.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/BoxComponent.h"
#include "FriendlyAI/AnimalMonster.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"

// TODO :: 리팩토링 중... -> 메시-콜리전 위치, 크기 안맞는 부분 수정,
// TODO :: AFieldActor로 대체할 수 있을듯
AAcidWaveActor::AAcidWaveActor()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh->SetupAttachment(GetRootComponent());
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	CollisionComp->SetupAttachment(GetRootComponent());
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AAcidWaveActor::OnOverlap);
}

void AAcidWaveActor::BeginPlay()
{
	Super::BeginPlay();
	CR4S_Log(LogDa, Warning, TEXT("[%s] BeginPlay - Damage : %f"), *MyHeader, Damage);
}

void AAcidWaveActor::Initialize(AActor* InOwner, AActor* InTarget)
{
	if (!CR4S_VALIDATE(LogDa, IsValid(InOwner)) || !CR4S_VALIDATE(LogDa, IsValid(InTarget))) return;

	OwnerActor = InOwner;
	TargetActor = InTarget;

	// if (auto BoxComp = Cast<UBoxComponent>(CollisionComp))
	// {
	// 	const float HalfHeight = BoxComp->GetScaledBoxExtent().Z;
	// 	FVector BoxLoc = BoxComp->GetRelativeLocation();
	// 	BoxLoc.Z = HalfHeight;
	// 	BoxComp->SetRelativeLocation(BoxLoc);
	// }
	if (auto BoxComp = Cast<UBoxComponent>(CollisionComp))
	{
		const float HalfHeight = BoxComp->GetScaledBoxExtent().Z;
		FVector MeshLoc = StaticMesh->GetRelativeLocation();
		MeshLoc.Z = HalfHeight;
		StaticMesh->SetRelativeLocation(MeshLoc);
	}
	
	if (bDynamicScale && IsValid(StaticMesh))
	{
		InitialScale = StaticMesh->GetRelativeScale3D();
		TargetScale = InitialScale + ScaleOffset;
		ElapsedScaleTime = 0.f;
	}

	if (bIsRotating)
		InitialRotation = GetActorRotation();

	ElapsedRotationTime = 0.f;
	ElapsedDelayTime = 0.f;
	
	const float TotalLifetime = RotationDuration + 2.f;
	FTimerHandle TmpHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TmpHandle,
		this,
		&AAcidWaveActor::CleanupSelf,
		TotalLifetime,
		false);

	CollisionComp->SetWorldTransform( StaticMesh->GetComponentTransform() );
}


void AAcidWaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDynamicScale && StaticMesh)
	{
		ElapsedScaleTime += DeltaTime;
		float AlphaScale = FMath::Clamp(ElapsedScaleTime / ScaleDuration, 0.f, 1.f);
		FVector NewScale = FMath::Lerp(InitialScale, TargetScale, AlphaScale);
		StaticMesh->SetRelativeScale3D(NewScale);

		if (AlphaScale >= 1.f)
			bDynamicScale = false;
	}

	if (ElapsedDelayTime < RotationStartDelay)
	{
		ElapsedDelayTime += DeltaTime;
		return;
	}
	
	if (bIsRotating)
	{
		ElapsedRotationTime += DeltaTime;
		float Alpha = FMath::Clamp(ElapsedRotationTime / RotationDuration, 0.f, 1.f);
		
		FRotator TargetRotation = InitialRotation + RotationOffset;
		FRotator NewRot;
		NewRot.Roll = FMath::Lerp(InitialRotation.Roll, TargetRotation.Roll, Alpha);
		NewRot.Pitch = FMath::Lerp(InitialRotation.Pitch, TargetRotation.Pitch, Alpha);
		NewRot.Yaw = FMath::Lerp(InitialRotation.Yaw, TargetRotation.Yaw, Alpha);
		
		StaticMesh->SetRelativeRotation(NewRot);
	
		if (Alpha >= 1.f)
			bIsRotating = false;
	}

	if (bDrawLine)
	{
		// NOTICE :: Test Log
		if (UBoxComponent* BoxComp = Cast<UBoxComponent>(CollisionComp))
		{
			FVector BoxCenter = BoxComp->GetComponentLocation();
			FVector BoxHalfExtent = BoxComp->GetScaledBoxExtent();
			FQuat   BoxQuat       = BoxComp->GetComponentQuat();
			DrawDebugBox(
				GetWorld(),
				BoxCenter,
				BoxHalfExtent,
				BoxQuat,
				FColor::Red,
				false,
				3.f,
				0,
				10.f
			);
		}
	}
}

void AAcidWaveActor::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	const int32 TotalTick = 10;
	DebuffTicksMap.Add(OtherActor, TotalTick);

	FTimerHandle NewHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AAcidWaveActor::ApplyPeriodicDamage, OtherActor);

	GetWorld()->GetTimerManager().SetTimer(NewHandle, TimerDel, 0.5f, true);

	DebuffTimerMap.Add(OtherActor, NewHandle);
}

void AAcidWaveActor::ApplyPeriodicDamage(AActor* AffectedActor)
{
	if (!AffectedActor) return;

	int32* RemainingTicksPtr = DebuffTicksMap.Find(AffectedActor);
	if (!RemainingTicksPtr) return;

	int32& RemainingTicks = *RemainingTicksPtr;
	if (RemainingTicks <= 0) return;

	if (Cast<ABaseMonster>(AffectedActor) || Cast<AAnimalMonster>(AffectedActor)) return;
	
	if (Damage > 0.f)
	{
		UGameplayStatics::ApplyDamage(AffectedActor, Damage, nullptr, OwnerActor, nullptr);
	}

	RemainingTicks--;

	if (RemainingTicks <= 0)
	{
		if (FTimerHandle* HandlePtr = DebuffTimerMap.Find(AffectedActor))
		{
			GetWorld()->GetTimerManager().ClearTimer(*HandlePtr);
		}
		DebuffTimerMap.Remove(AffectedActor);
		DebuffTicksMap.Remove(AffectedActor);
	}
}

void AAcidWaveActor::CleanupSelf()
{
	for (auto& Pair : DebuffTimerMap)
	{
		GetWorld()->GetTimerManager().ClearTimer(Pair.Value);
	}
	DebuffTimerMap.Empty();
	DebuffTicksMap.Empty();

	Destroy();
}


