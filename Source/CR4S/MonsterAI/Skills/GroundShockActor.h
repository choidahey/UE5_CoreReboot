#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundShockActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UCapsuleComponent;

UCLASS()
class CR4S_API AGroundShockActor : public AActor
{
	GENERATED_BODY()

public:
	AGroundShockActor();
	void InitShock(const FRotator& FacingRot);

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> NiagaraComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> NiagaraPerCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Shock")
	float MinRadius = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Shock")
	float MaxRadius = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Shock")
	int32 LevelCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Shock")
	float AngleRange = 60.f;

	UPROPERTY(EditAnywhere)
	float CapsuleRadius = 100.f;

	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 80.f;

	UPROPERTY(EditAnywhere)
	float CoverageFactor = 0.8f;

	UPROPERTY(EditAnywhere)
	float Damage = 100.f;

	UPROPERTY(EditAnywhere)
	float LifeTime = 1.5f;

	UPROPERTY()
	TArray<TObjectPtr<UCapsuleComponent>> ArcColliders;
};
