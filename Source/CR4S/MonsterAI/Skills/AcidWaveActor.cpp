#include "MonsterAI/Skills/AcidWaveActor.h"
#include "CR4S.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"

AAcidWaveActor::AAcidWaveActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	BoxCollisionComp->SetupAttachment(RootComp);
	BoxCollisionComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	BoxCollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	BoxCollisionComp->SetGenerateOverlapEvents(true);
	BoxCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AAcidWaveActor::OnOverlapBegin);
}

void AAcidWaveActor::BeginPlay()
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

void AAcidWaveActor::Initialize(AActor* InOwner, AActor* InTarget)
{
	if (!CR4S_VALIDATE(LogDa, IsValid(InOwner)) || !CR4S_VALIDATE(LogDa, IsValid(InTarget))) return;

	OwnerActor = InOwner;
	TargetActor = InTarget;
	
	if (MeshComp && MeshComp->GetStaticMesh())
	{
		const FVector LocalExtent = MeshComp->GetStaticMesh()->GetBoundingBox().GetExtent();
		const float HalfHeight = LocalExtent.Z * MeshComp->GetComponentScale().Z;
		MeshComp->SetRelativeLocation(FVector(0.f, 0.f, HalfHeight));
	}

	if (BoxCollisionComp)
	{
		float BoxHalfHeight = 0.f;
		if (MeshComp && MeshComp->GetStaticMesh())
		{
			const FVector LocalExtent = MeshComp->GetStaticMesh()->GetBoundingBox().GetExtent();
			BoxHalfHeight = LocalExtent.Z * MeshComp->GetComponentScale().Z;
		}
		BoxCollisionComp->SetRelativeLocation(FVector(0.f, 0.f, BoxHalfHeight));
	}

	if (bDynamicScale && IsValid(MeshComp) && IsValid(BoxCollisionComp))
	{
		InitialScale = MeshComp->GetRelativeScale3D();
		InitialBoxExtent = BoxCollisionComp->GetUnscaledBoxExtent();
		TargetScale = InitialScale + ScaleOffset;
		ElapsedScaleTime = 0.f;
	}

	if (bIsRotating)
	{
		InitialRotation = GetActorRotation();
	}

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
}


void AAcidWaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDynamicScale && MeshComp)
	{
		ElapsedScaleTime += DeltaTime;
		float AlphaScale = FMath::Clamp(ElapsedScaleTime / ScaleDuration, 0.f, 1.f);
		FVector NewScale = FMath::Lerp(InitialScale, TargetScale, AlphaScale);
		MeshComp->SetRelativeScale3D(NewScale);

		FVector NewExtent = InitialBoxExtent * NewScale;
		BoxCollisionComp->SetBoxExtent(NewExtent);

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
		NewRot.Roll  = FMath::Lerp(InitialRotation.Roll,  TargetRotation.Roll,  Alpha);
		NewRot.Pitch = FMath::Lerp(InitialRotation.Pitch, TargetRotation.Pitch, Alpha);
		NewRot.Yaw   = FMath::Lerp(InitialRotation.Yaw,   TargetRotation.Yaw,   Alpha);
	
		SetActorRotation(NewRot);
	
		if (Alpha >= 1.f)
		{
			bIsRotating = false;
		}
	}
}

void AAcidWaveActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == OwnerActor) return;
	if (OtherActor != TargetActor) return;

	if (Damage > 0.f)
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, OwnerActor, nullptr);

	const int32 TotalTick = 10;
	DebuffTicksMap.Add(OtherActor, TotalTick);

	FTimerHandle NewHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AAcidWaveActor::ApplyPeriodicDamage, OtherActor);

	GetWorld()->GetTimerManager().SetTimer(
		NewHandle,
		TimerDel,
		0.5f,
		true);

	DebuffTimerMap.Add(OtherActor, NewHandle);
		
}

void AAcidWaveActor::ApplyPeriodicDamage(AActor* AffectedActor)
{
	if (!AffectedActor) return;

	int32* RemainingTicksPtr = DebuffTicksMap.Find(AffectedActor);
	if (!RemainingTicksPtr) return;

	int32& RemainingTicks = *RemainingTicksPtr;
	if (RemainingTicks <= 0) return;

	if (Cast<ABaseMonster>(AffectedActor)) return;
	
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

