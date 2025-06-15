#pragma once

#include "CoreMinimal.h"
#include "ObjectPoolable.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AnimalProjectile.generated.h"

UCLASS()
class CR4S_API AAnimalProjectile : public AObjectPoolable
{
	GENERATED_BODY()

public:
	AAnimalProjectile();

protected:
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
						 UPrimitiveComponent* OtherComp, FVector NormalImpulse,
						 const FHitResult& Hit);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage;
	
	void OnSpawnFromPool() override;
	
	void OnReturnToPool() override;
};
