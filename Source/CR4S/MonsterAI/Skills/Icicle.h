#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Icicle.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UCapsuleComponent;

UCLASS()
class CR4S_API AIcicle : public AActor
{
	GENERATED_BODY()

public:
	AIcicle();

	void InitIcicle(float Radius);

protected:
	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)
	USceneComponent* RootComp;

	//UPROPERTY(EditAnywhere)
	//UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* NiagaraTemplate;


	UPROPERTY()
	TArray<UCapsuleComponent*> EdgeColliders;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Icicle|Collision")
	float CapsuleRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Icicle|Collision")
	float CapsuleHalfHeight = 80.f;

	UPROPERTY(EditAnywhere, Category = "Icicle|Collision")
	float CoverageFactor = 0.8f;
};
