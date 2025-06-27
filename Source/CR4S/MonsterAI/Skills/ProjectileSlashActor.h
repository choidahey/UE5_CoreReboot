#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "ProjectileSlashActor.generated.h"

class UNiagaraComponent;
class UBoxComponent;

UCLASS()
class CR4S_API AProjectileSlashActor : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:
	AProjectileSlashActor();
	virtual void Tick(float DeltaTime) override;

	void InitializeMovement(float InSpeed, float InDistance, const FVector& InStartLocation);

protected:
	virtual void BeginPlay() override;
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* TrailEffect;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* DestroyEffect;

	/* Slash movement */
	FVector MoveDirection = FVector::ZeroVector;
	float MoveSpeed = 1000.f;
	float MaxTravelDistance = 3000.f;
	FVector StartLocation;

};
