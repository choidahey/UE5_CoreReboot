#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightningStrikeActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UCapsuleComponent;

UCLASS()
class CR4S_API ALightningStrikeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALightningStrikeActor();

	void InitializeStrike(const FVector& TargetLocation, UNiagaraSystem* LightningEffect, float );

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY()
	USceneComponent* RootComp;

	UPROPERTY()
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Lightning|Collision")
	UCapsuleComponent* LightningCollider;

	UPROPERTY()
	FVector TargetPosition;

	UPROPERTY()
	float Damage = 200.f;

	UPROPERTY(EditAnywhere, Category = "Lightning|Collision")
	float CapsuleSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Lightning|Collision")
	float CapsuleRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Lightning|Collision")
	float CapsuleHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Lightning")
	float ArrivalThreshold = 10;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> DamagedActors;

};
