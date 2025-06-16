#include "MonsterAI/Skills/GroundShockActor.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AGroundShockActor::AGroundShockActor()
{
	if (NiagaraComp)
	{
		NiagaraComp->SetupAttachment(RootComp);
		NiagaraComp->SetAutoActivate(false);
	}
}

void AGroundShockActor::InitShock(const FRotator& FacingRot)
{
	APawn* InstigatorPawn = Cast<APawn>(GetInstigator());
	if (InstigatorPawn)
	{
		if (UMonsterSkillComponent* SkillComp = InstigatorPawn->FindComponentByClass<UMonsterSkillComponent>())
		{
			Damage = SkillComp->GetCurrentSkillData().Damage;
		}
	}

	const FVector BaseLocation = GetActorLocation();
	const float StartAngleDeg = -AngleRange * 0.5f;
	const float EndAngleDeg = +AngleRange * 0.5f;

	for (int32 Level = 0; Level < LevelCount; ++Level)
	{
		float Radius = FMath::Lerp(MinRadius, MaxRadius, float(Level) / FMath::Max(1, LevelCount - 1));
		float ArcLength = 2.f * PI * Radius * (AngleRange / 360.f);
		float Coverage = (CapsuleRadius * 2.f) * CoverageFactor;
		int32 NumSeg = FMath::Max(3, FMath::CeilToInt(ArcLength / Coverage));

		for (int32 i = 0; i < NumSeg; ++i)
		{
			float AngleDeg = FMath::Lerp(StartAngleDeg, EndAngleDeg, float(i) / (NumSeg - 1));
			float AngleRad = FMath::DegreesToRadians(AngleDeg);

			FVector LocalDir = FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f);
			FVector RotatedDir = FacingRot.RotateVector(LocalDir);
			FVector Pos = BaseLocation + RotatedDir * Radius;

			// Ground Line Trace
			FVector Start = Pos + FVector(0.f, 0.f, TraceHeightAbove);
			FVector End = Pos - FVector(0.f, 0.f, TraceDepthBelow);

			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			if (GetInstigator())
			{
				Params.AddIgnoredActor(GetInstigator());
			}

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
			{
				Pos.Z = HitResult.ImpactPoint.Z;
			}

			// Create Capsule Components
			UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(this);
			Capsule->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
			Capsule->SetCollisionProfileName(TEXT("MonsterSkillActor"));
			Capsule->SetGenerateOverlapEvents(true);
			Capsule->OnComponentBeginOverlap.AddDynamic(this, &AGroundShockActor::OnOverlap);

			Capsule->SetupAttachment(RootComp);
			Capsule->RegisterComponent();
			Capsule->SetWorldLocation(Pos);
			Capsule->SetWorldRotation(RotatedDir.Rotation());

			ArcColliders.Add(Capsule);

			if (NiagaraPerCapsule)
			{
				UNiagaraComponent* Niagara = NewObject<UNiagaraComponent>(this);
				Niagara->SetAsset(NiagaraPerCapsule);
				Niagara->SetupAttachment(Capsule);
				Niagara->SetRelativeLocation(FVector::ZeroVector);
				Niagara->SetRelativeRotation(FRotator::ZeroRotator);
				Niagara->SetRelativeScale3D(FVector(CapsuleRadius / 100.f));

				Niagara->RegisterComponent();
				Niagara->Activate(true);
			}

#if WITH_EDITOR
			DrawDebugCapsule(
				GetWorld(),
				Pos + FVector(0, 0, CapsuleHalfHeight),
				CapsuleHalfHeight,
				CapsuleRadius,
				FQuat(RotatedDir.Rotation()),
				FColor::Orange,
				false,
				LifeTime,
				0,
				1.f
			);
#endif
		}
	}

	SetLifeSpan(LifeTime);
}

void AGroundShockActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}
