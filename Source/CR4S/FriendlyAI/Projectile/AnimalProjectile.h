#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AnimalProjectile.generated.h"

class UObjectPoolComponent;
class UCapsuleComponent;
class UNiagaraComponent;

UCLASS()
class CR4S_API AAnimalProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAnimalProjectile();
	virtual void BeginPlay() override;
	void SetProjectileDamage(float InDamage) { Damage = InDamage; }

protected:
	UFUNCTION()
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ResetProjectile();

	UFUNCTION()
	void OnNiagaraFinished(UNiagaraComponent* PSystem);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UObjectPoolComponent* PoolComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent* HitEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNiagaraComponent* ProjectileNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage;
	
};