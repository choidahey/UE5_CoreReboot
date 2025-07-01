#pragma once

#include "CoreMinimal.h"
#include "BaseSkillActor.h"
#include "ProjectileBomb.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class CR4S_API AProjectileBomb : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	AProjectileBomb();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;
	
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;

	UFUNCTION()
	void OnExplosionOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	void LaunchProjectile();

	UPROPERTY(VisibleAnywhere, Category = "Boss|Skill")
	UProjectileMovementComponent* ProjectileMovement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	FVector ExplosionScale = FVector(1.f, 1.f, 1.f);
	
	UPROPERTY(VisibleAnywhere, Category="Boss|Skills")
    USphereComponent* ExplosionOverlapComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float ActorLifeTime = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill", meta = (AllowPrivateAccess = "true"))
	float LaunchDelayTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill", meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="90.0"))
	float LaunchPitchAngle = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float ExplosionRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float ExplosionDamage = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float MaxHealth = 5000.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Skill")
	float CurrentHealth;


private:
	void ApplyPeriodicDamage(AActor* Victim);

	UPROPERTY()
	TMap<AActor*, FTimerHandle> OverlapDamageTimers;

	FTimerHandle LaunchTimerHandle;
};