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
	void InitIcicleWaves(
		int32 SpawnCount,
		float BaseDistance,
		float DistanceStep,
		float Interval,
		bool bForwardIn
	);

protected:
	void SpawnIcicleWaves();

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Skill")
	TSubclassOf<AActor> IcicleActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float IcicleColliderRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float CoverageFactor = 0.8f;

private:
	TArray<float> SpawnRadius;
	float SpawnInterval;
	int32 SpawnIndex;
	bool bForward;
	FVector SpawnCenterLocation;
	
	FTimerHandle SpawnTimerHandle;
};
