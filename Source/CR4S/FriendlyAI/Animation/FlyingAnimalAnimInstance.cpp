#include "FlyingAnimalAnimInstance.h"
#include "../Component/FlyingMovementComponent.h"

void UFlyingAnimalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn) return;

	UFlyingMovementComponent* FlyComp = OwningPawn->FindComponentByClass<UFlyingMovementComponent>();
	if (FlyComp)
	{
		bIsFlying       = FlyComp->bIsFlying;
		bIsWalking      = !FlyComp->bIsFlying;
		bIsAir          = FlyComp->bIsFlying;
		WalkingSpeed    = FlyComp->GetOwner()->GetVelocity().Size();
		Direction       = FlyComp->CurrentDir.Y;
		bFlapFast       = FlyComp->bFlapFast;
		bFlyingDownward = FlyComp->bFlyingDownward;
		bIdleLoop       = FlyComp->bIdleLoop;
		RandomIdleAnim  = FlyComp->RandomIdleAnim;
	}
	else
	{
		bIsFlying       = 0;
		bIsWalking      = 1;
		bIsAir          = 0;
		WalkingSpeed    = 0.f;
		Direction       = 0.f;
		bFlapFast       = 0;
		bFlyingDownward = 0;
		bIdleLoop       = 0;
		RandomIdleAnim  = 0;
	}
}