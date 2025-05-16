#include "AnimalAnimInstance.h"
#include "../BaseAnimal.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimalAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
		OwnerAnimal = Cast<ABaseAnimal>(OwnerCharacter);
	}
}

void UAnimalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner)
	{
		GroundSpeed = 0.f;
		bShouldMove = false;
		return;
	}

	OwnerCharacter = Cast<ABaseAnimal>(PawnOwner);
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
}

void UAnimalAnimInstance::AnimNotify_AnimalAttackHit()
{
	if (OwnerAnimal)
	{
		OwnerAnimal->PerformAttack();
	}
}