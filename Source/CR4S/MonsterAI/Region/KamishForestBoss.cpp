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

	const float CloudSizeParam = CombatRange / NiagaraRadiusBase;
	const float AdjustedHeight = CloudHeight * CloudSizeParam;

	const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, CloudHeight);
	CloudOriginLocation = SpawnLocation;

	ActiveCloudEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		CloudAsset,
		SpawnLocation,
		GetActorRotation(),
		FVector(1.f),
		true,
		true,
		ENCPoolMethod::None,
		true
	);

	if (ActiveCloudEffect)
	{
		ActiveCloudEffect->SetVariableFloat(FName("User_CloudSize"), CloudSizeParam);
	}
}

void AKamishForestBoss::DestroyActiveClouds()
{
	if (IsValid(ActiveCloudEffect))
	{
		ActiveCloudEffect->DestroyComponent();
		ActiveCloudEffect = nullptr;
	}
}

