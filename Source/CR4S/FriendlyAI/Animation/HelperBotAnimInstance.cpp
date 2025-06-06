#include "HelperBotAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../BaseHelperBot.h"

void UHelperBotAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	OwnerCharacter = Cast<ABaseHelperBot>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UHelperBotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner)
	{
		GroundSpeed = 0.f;
		bShouldMove = false;
		return;
	}

	OwnerCharacter = Cast<ABaseHelperBot>(PawnOwner);
	if (!OwnerCharacter)
	{
		GroundSpeed = 0.f;
		bShouldMove = false;
		return;
	}

	OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	if (!OwnerCharacterMovementComponent)
	{
		GroundSpeed = 0.f;
		bShouldMove = false;
		return;
	}

	FVector HorizontalVelocity = OwnerCharacter->GetVelocity();
	HorizontalVelocity.Z = 0.f;
	GroundSpeed = HorizontalVelocity.Size();
	bShouldMove = GroundSpeed > 3.f && !OwnerCharacterMovementComponent->IsFalling();
	bIsJumping = OwnerCharacterMovementComponent->IsFalling();
	bIsChopping = OwnerCharacter->bIsChopping;
	bIsFalling = OwnerCharacterMovementComponent->IsFalling();
}
