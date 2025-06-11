#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "LightningStrikeActor.generated.h"

UCLASS()
class CR4S_API ALightningStrikeActor : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:	
	ALightningStrikeActor();

	void InitializeStrike(const FVector& TargetLocation, UNiagaraSystem* LightningEffect, float );

protected:
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	UFUNCTION()
	void HandleSelfDestruct();
	
	UPROPERTY(EditAnywhere, Category = "Lightning|Collision")
	float LightningWidth = 20.f;

	UPROPERTY(EditAnywhere, Category = "Lightning|Collision")
	float CapsuleRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Lightning")
	float LightningDelayBeforeStrike = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Lightning")
	float ActorLifeAfterStrike = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Lightning|Trace")
	float TraceHeightAbove = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Lightning|Trace")
	float TraceDepthBelow = 2000.f;

};
