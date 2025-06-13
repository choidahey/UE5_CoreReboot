#pragma once

#include "CoreMinimal.h"
#include "BaseSkillActor.h"
#include "GameFramework/Actor.h"
#include "ThunderWall.generated.h"

class UNiagaraSystem;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class CR4S_API AThunderWall : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	AThunderWall();

protected:
	virtual void BeginPlay() override;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallSkill")
	// UNiagaraComponent* NiagaraComp;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallSkill")
	// UBoxComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallSkill|Dimensions")
	FVector WallExtent = FVector(50.f, 500.f, 800.f);

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallSkill|Damage")
	// float Damage = 0.f;

private:
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
};
