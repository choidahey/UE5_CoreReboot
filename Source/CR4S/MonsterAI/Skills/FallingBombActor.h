#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "FallingBombActor.generated.h"

class UCapsuleComponent;
class UNiagaraSystem;

UCLASS()
class CR4S_API AFallingBombActor : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	AFallingBombActor();

	void Initialize(AActor* InOwner, AActor* InTarget);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp,	AActor* OtherActor,	UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnLandingDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void TriggerExplosion();
	void SpawnExplosionEffect();
	void SpawnDirectionalBursts();
	void ShatterOnly();

	UPROPERTY(VisibleAnywhere, Category = "Projectile|Collision")
	TObjectPtr<UCapsuleComponent> LandingTrigger;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb")
	TObjectPtr<UNiagaraSystem> ShatterEffect;

	UPROPERTY(EditAnywhere, Category = "FallingBomb|Spawn")
	float MinSpawnHeight = 800.f;

	UPROPERTY(EditAnywhere, Category = "FallingBomb|Spawn")
	float MaxSpawnHeight = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb|Spawn")
	float RandomSpawnRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "FallingBomb|Gravity")
	float GravityAcceleration = -980.f;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb|Explosion")
	float ExplosionDelay = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb|Explosion")
	float ExplosionRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb|Explosion")
	float ExplosionAngle = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "FallingBomb|Projectile")
	TSubclassOf<AActor> ShardClass;

	UPROPERTY(EditAnywhere, Category = "FallingBomb|Projectile")
	int32 MaxBurstCount = 16;

	UPROPERTY(EditAnywhere, Category = "FallingBomb|Projectile")
	float TraceCheckLength = 500.f;

	UPROPERTY(EditAnywhere, Category = "FallingBomb|Projectile")
	float LaunchSpeed = 1500.f;

private:
	UPROPERTY()
	FTimerHandle ExplosionTimerHandle;

	float CurrentFallSpeed = 0.f;
	bool bHasLanded = false;
	bool bHasExploded = false;

};
