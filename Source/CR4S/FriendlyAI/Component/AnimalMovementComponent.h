#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimalMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAnimalMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void MoveToLocation(const FVector& Dest) PURE_VIRTUAL(UAnimalMovementComponent::MoveToLocation, );
    
	UFUNCTION(BlueprintCallable)
	virtual void MoveToActor(AActor* TargetActor) PURE_VIRTUAL(UAnimalMovementComponent::MoveToActor, );
    
	UFUNCTION(BlueprintCallable)
	virtual void StopMovement() PURE_VIRTUAL(UAnimalMovementComponent::StopMovement, );

	virtual bool HasReachedDestination(const FVector& Dest, float AcceptanceRadius) const { return false; }
};