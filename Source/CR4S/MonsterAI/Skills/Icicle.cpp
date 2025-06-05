#include "MonsterAI/Skills/Icicle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AIcicle::AIcicle()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->SetAutoActivate(false);
	NiagaraComp->Activate(true);
}

void AIcicle::InitIcicle(float Radius)
{
	if (APawn* OwnerPawn = GetInstigator())
	{
		if (UMonsterSkillComponent* SkillComp = OwnerPawn->FindComponentByClass<UMonsterSkillComponent>())
		{
			Damage = SkillComp->GetCurrentSkillData().Damage;
		}
	}

	for (UCapsuleComponent* C : EdgeColliders)
		if (C) C->DestroyComponent();

	EdgeColliders.Empty();

	const FVector BaseLoc = GetActorLocation();
    const float BottomZ = BaseLoc.Z;
    const float CenterZ = BottomZ + CapsuleHalfHeight;
    const float CircleAngle = 2.f * PI;

    const float Circumference = 2.f * PI * Radius;
    const float CoveragePerCapsule = (CapsuleRadius * 2.f) * CoverageFactor;
    int32 NumSeg = FMath::CeilToInt(Circumference / CoveragePerCapsule);
    NumSeg = FMath::Max(3, NumSeg);

    for (int32 i = 0; i < NumSeg; ++i)
    {
        float Angle = CircleAngle * float(i) / float(NumSeg);
        FVector Dir = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f);

        FVector LocXY = FVector(BaseLoc.X + Dir.X * Radius,
                                BaseLoc.Y + Dir.Y * Radius,
                                0.f);
    	
        float GroundZ = BottomZ;
        {
            FVector TraceStart = FVector(LocXY.X, LocXY.Y, BaseLoc.Z + TraceHeight);
            FVector TraceEnd   = FVector(LocXY.X, LocXY.Y, BaseLoc.Z - TraceHeight);

            FHitResult Hit;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);
            if (GetInstigator())
                Params.AddIgnoredActor(GetInstigator());

            if (GetWorld()->LineTraceSingleByChannel(
                    Hit,
                    TraceStart,
                    TraceEnd,
                    ECC_WorldStatic,
                    Params))
            {
                GroundZ = Hit.Location.Z;
            }
        }
    	
        const FVector CapsuleCenter = FVector(LocXY.X, LocXY.Y, CenterZ);

        UCapsuleComponent* CapsuleComp = NewObject<UCapsuleComponent>(this);
        CapsuleComp->AttachToComponent(RootComp, FAttachmentTransformRules::KeepWorldTransform);
        CapsuleComp->RegisterComponent();

        CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
        CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

        CapsuleComp->SetWorldLocation(CapsuleCenter);
        CapsuleComp->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
        CapsuleComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

        CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AIcicle::OnOverlap);
        EdgeColliders.Add(CapsuleComp);
	    
        {
            const FQuat CapsuleRot = FQuat::Identity;
            const float DrawDuration = 1.5f;
            const float LineThickness = 2.f;
            const FColor DrawColor = FColor::Green;

            DrawDebugCapsule(
                GetWorld(),
                CapsuleCenter,
                CapsuleHalfHeight,
                CapsuleRadius,
                CapsuleRot,
                DrawColor,
                false,
                DrawDuration,
                0,
                LineThickness
            );
        }
    	
        if (NiagaraTemplate)
        {
            FVector LocalOffset = FVector(Dir.X * Radius,Dir.Y * Radius, GroundZ - BaseLoc.Z);

            UNiagaraComponent* NewNiComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                NiagaraTemplate,
                RootComp,
                NAME_None,
                LocalOffset,
                FRotator::ZeroRotator,
                EAttachLocation::KeepRelativeOffset,
                true,
                true,
                ENCPoolMethod::None,
                false
            );

            NewNiComp->SetRelativeScale3D(NiagaraScale);
            SpawnedNiagaras.Add(NewNiComp);
        }
    }

    SetLifeSpan(LifeTime);
}

void AIcicle::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner() || OtherActor == GetInstigator()) return;

	if (OtherActor->FindComponentByClass<UMonsterAttributeComponent>())
		return;

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);
}

void AIcicle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SpawnedNiagaras.Num() <= 0) return;
	
	for (UNiagaraComponent* NiComp : SpawnedNiagaras)
	{
		if (IsValid(NiComp))
		{
			NiComp->DestroyComponent();
		}
	}
	SpawnedNiagaras.Empty();
}
