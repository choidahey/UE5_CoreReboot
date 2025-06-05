#include "GroundMovementComponent.h"
#include "AIController.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

void UGroundMovementComponent::MoveToLocation(const FVector& Dest)
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (AAIController* AICon = Cast<AAIController>(OwnerPawn->GetController()))
		{
			AICon->MoveToLocation(Dest);
		}
	}
}

void UGroundMovementComponent::MoveToActor(AActor* TargetActor)
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (AAIController* AICon = Cast<AAIController>(OwnerPawn->GetController()))
		{
			AICon->MoveToActor(TargetActor);
		}
	}
}

void UGroundMovementComponent::StopMovement()
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (AAIController* AICon = Cast<AAIController>(OwnerPawn->GetController()))
		{
			AICon->StopMovement();
		}
	}
}

bool UGroundMovementComponent::HasReachedDestination(const FVector& Dest, float AcceptanceRadius) const
{
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		const float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), Dest);
		return Distance <= AcceptanceRadius;
	}
	return false;
}
