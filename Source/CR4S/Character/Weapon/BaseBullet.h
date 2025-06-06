// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

struct FBulletData;
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
	void Initialize(const FBulletData& InData);
	
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
	float Damage{100};
	
#pragma region Components
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<USceneComponent> SceneComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UBoxComponent> CollisionComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
#pragma endregion

#pragma region Properties
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Explosion")
	float ExplosionRadius{0};
#pragma endregion
	
#pragma region Effects
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UParticleSystem> ImpactParticle;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSound;
#pragma endregion
};
