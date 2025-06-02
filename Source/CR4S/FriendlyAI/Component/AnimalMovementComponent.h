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
	virtual void MoveToLocation(const FVector& Dest);
	
	UFUNCTION(BlueprintCallable)
	virtual void MoveToActor(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable)
	virtual void StopMovement();

	bool HasReachedDestination(const FVector& Dest, float AcceptanceRadius) const;
};