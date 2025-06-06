#include "KamishForestBoss.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
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
		SpawnCloudEffect();
		SkillComponent->UseSkill(0);
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

	const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, CloudVerticalOffset);

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
		const float SizeParam = CloudVisualRadius / NiagaraRadiusBase;
		// ActiveCloudEffect->SetVariableFloat(FName("User_CloudSize"), SizeParam);
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

