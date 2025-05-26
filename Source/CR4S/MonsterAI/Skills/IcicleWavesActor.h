#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IcicleWavesActor.generated.h"

class AIcicle;

UCLASS()
class CR4S_API AIcicleWavesActor : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Boss|Skill")
	void SpawnIcicleWaves(
		int32 SpawnCount,
		float BaseDistance,
		float DistanceStep,
		float Interval,
		bool bForwardIn
	);

protected:
	virtual void BeginPlay() override;

private:
	void SpawnNext();

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Skill")
	TSubclassOf<AIcicle> IcicleActorClass;

	TArray<float> SpawnRadius;
	float SpawnInterval;
	int32 SpawnIndex;
	FTimerHandle SpawnTimerHandle;
	bool bForward;
	FVector SpawnCenterLocation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float IcicleColliderRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float CoverageFactor = 0.8f;
};
