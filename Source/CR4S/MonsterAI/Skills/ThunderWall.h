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
	virtual void Tick(float DeltaTime) override;

private:
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	UPROPERTY()
	AActor* BossActor = nullptr;;
    
	UPROPERTY()
	FRotator TargetRotation = FRotator::ZeroRotator;
};
