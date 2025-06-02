#include "AnimalMovementComponent.h"
#include "Engine/World.h"

void UAnimalMovementComponent::MoveToLocation(const FVector& Dest)
{
}

void UAnimalMovementComponent::MoveToActor(AActor* TargetActor)
{
}

void UAnimalMovementComponent::StopMovement()
{
}

bool UAnimalMovementComponent::HasReachedDestination(const FVector& Dest, float AcceptanceRadius) const
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		const float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), Dest);
		return Distance <= AcceptanceRadius;
	}
	return false;
}