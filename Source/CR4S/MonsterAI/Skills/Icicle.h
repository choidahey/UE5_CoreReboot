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
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	UPROPERTY(EditAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* NiagaraTemplate;

	UPROPERTY()
	TArray<UCapsuleComponent*> EdgeColliders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	FVector NiagaraScale = FVector(1.f, 1.f, 1.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float CapsuleRadius = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float CapsuleHalfHeight = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float CoverageFactor = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float LifeTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float TraceHeight = 2000.f;

private:
	UPROPERTY()
	TArray<UNiagaraComponent*> SpawnedNiagaras;
	
	FString MyHeader = TEXT("Icicle");
};
