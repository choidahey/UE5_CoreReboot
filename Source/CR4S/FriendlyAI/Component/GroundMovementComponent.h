#pragma once

#include "CoreMinimal.h"
#include "AnimalMovementComponent.h"
#include "GroundMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UGroundMovementComponent : public UAnimalMovementComponent
{
	GENERATED_BODY()

public:
	virtual void MoveToLocation(const FVector& Dest) override;
	virtual void MoveToActor(AActor* TargetActor) override;
	virtual void StopMovement() override;
};