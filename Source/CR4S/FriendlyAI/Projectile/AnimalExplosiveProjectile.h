#pragma once

#include "CoreMinimal.h"
#include "AnimalProjectile.h"
#include "AnimalExplosiveProjectile.generated.h"

UCLASS()
class CR4S_API AAnimalExplosiveProjectile : public AAnimalProjectile
{
	GENERATED_BODY()

public:
	AAnimalExplosiveProjectile();

protected:
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								  bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DealDotDamage();

	UFUNCTION()
	void DeactivateExplosion();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* ExplosionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	bool bIsDotDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DotInterval = 1.0f;

private:
	UPROPERTY()
	TArray<AActor*> DamagedActors;

	FTimerHandle ExplosionTimerHandle;
	FTimerHandle DotDamageTimerHandle;
};