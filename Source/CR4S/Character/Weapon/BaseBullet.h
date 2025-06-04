// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class URadialForceComponent;
class UProjectileMovementComponent;

UCLASS()
class CR4S_API ABaseBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseBullet();

public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Damage")
	float BaseDamage;
	
#pragma region Components
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	UBoxComponent* CollisionComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	UProjectileMovementComponent* ProjectileMovementComponent;
#pragma endregion

#pragma region Effects
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Effects")
	USoundBase* ImpactSound;
#pragma endregion
};
