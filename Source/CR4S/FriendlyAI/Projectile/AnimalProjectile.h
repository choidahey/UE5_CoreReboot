#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AnimalProjectile.generated.h"

class UObjectPoolComponent;

UCLASS()
class CR4S_API AAnimalProjectile : public AActor
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
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UObjectPoolComponent* PoolComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage;
};