#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "ExplosionActor.generated.h"

class AProjectileShard;

UCLASS()
class CR4S_API AExplosionActor : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:
	AExplosionActor();

protected:
	virtual void BeginPlay() override;
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	void ApplyShockwaveDamage();
	void SpawnShardProjectiles();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Shockwave")
	float ShockwaveRadius = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion|Shard")
	TSubclassOf<AProjectileShard> ShardProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Shard")
	int32 ShardCount = 12;

	UPROPERTY(EditAnywhere, Category = "Explosion|Shard")
	float ShardSpreadAngle = 105.f;

	UPROPERTY(EditAnywhere, Category = "Explosion|Shard")
	float LaunchSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Explosion|Shard")
	float LaunchDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion|FX")
	TObjectPtr<UNiagaraSystem> ExplosionRadiusEffect;

};
