#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "KamishForestBoss.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class CR4S_API AKamishForestBoss : public ARegionBossMonster
{
	GENERATED_BODY()

public:
	AKamishForestBoss();

protected:
	virtual void OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current) override;
	virtual void HandleDeath() override;

	void SpawnCloudEffect();
	void DestroyActiveClouds();
	
public:
	FORCEINLINE FVector GetCloudOriginLocation() const { return CloudOriginLocation; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Kamish|Cloud")
	TObjectPtr<UNiagaraSystem> CloudAsset;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveCloudEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kamish|Cloud")
	FVector CloudOriginLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Kamish|Cloud")
	float CloudHeight = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Kamish|Cloud")
	float NiagaraRadiusBase = 100.0f;

};
