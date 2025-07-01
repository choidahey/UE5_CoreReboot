#pragma once

#include "CoreMinimal.h"
#include "AnimalProjectile.h"
#include "AnimalSingleTargetProjectile.generated.h"

UCLASS()
class CR4S_API AAnimalSingleTargetProjectile : public AAnimalProjectile
{
	GENERATED_BODY()

public:
	AAnimalSingleTargetProjectile();

protected:
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								  bool bFromSweep, const FHitResult& SweepResult) override;
};