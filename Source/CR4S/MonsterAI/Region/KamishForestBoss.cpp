#include "KamishForestBoss.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AKamishForestBoss::AKamishForestBoss()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKamishForestBoss::OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current)
{
	Super::OnMonsterStateChanged(Previous, Current);

	if (Current == EMonsterState::Combat)
	{
		AnimComponent->PlayCombatMontage();
		SpawnCloudEffect();
	}

	if (Previous == EMonsterState::Combat && Current != EMonsterState::Combat)
	{
		DestroyActiveClouds();
	}
}

void AKamishForestBoss::HandleDeath()
{
	DestroyActiveClouds();
	Super::HandleDeath();
}

void AKamishForestBoss::SpawnCloudEffect()
{
	if (!CloudAsset || ActiveCloudEffect) return;

	const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, CloudHeight);
	CloudOriginLocation = SpawnLocation;

	const float NiagaraScale = CombatRange / NiagaraRadiusBase;
	FVector NiagaraScale3D(NiagaraScale);

	ActiveCloudEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		CloudAsset,
		SpawnLocation,
		GetActorRotation(),
		NiagaraScale3D,
		true,
		true,
		ENCPoolMethod::None,
		true
	);

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		CloudOriginLocation,
		CombatRange,
		32,
		FColor::Cyan,
		false,
		10.f,  // 10초간 유지 (혹은 -1.f: 무제한)
		0,
		3.f
	);

	DrawDebugString(
		GetWorld(),
		CloudOriginLocation + FVector(0, 0, 100.f),
		TEXT("SwirlCloudOrigin"),
		nullptr,
		FColor::White,
		10.f,
		true
	);
#endif
}

void AKamishForestBoss::DestroyActiveClouds()
{
	if (IsValid(ActiveCloudEffect))
	{
		ActiveCloudEffect->DestroyComponent();
		ActiveCloudEffect = nullptr;
	}
}

