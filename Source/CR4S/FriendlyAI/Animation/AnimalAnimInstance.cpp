#include "AnimalAnimInstance.h"
#include "../BaseAnimal.h"
#include "Kismet/KismetMathLibrary.h"
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
		OwnerAnimal->PerformMeleeAttack();
	}
}

void UAnimalAnimInstance::AnimNotify_AnimalDash()
{
	if (OwnerAnimal)
	{
		OwnerAnimal->PerformChargeAttack();
	}
}

void UAnimalAnimInstance::AnimNotify_AnimalRanged()
{
	if (OwnerAnimal)
	{
		OwnerAnimal->PerformRangedAttack();
	}
}

bool UAnimalAnimInstance::PlayRandomIdleMontage()
{
	if (IdleMontages.Num() == 0) return false;

	int32 Index = UKismetMathLibrary::RandomIntegerInRange(0, IdleMontages.Num() - 1);
	UAnimMontage* Chosen = IdleMontages[Index];
	if (!Chosen) return false;

	float Duration = Montage_Play(Chosen);
	if (Duration > 0.f)
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUFunction(this, TEXT("HandleMontageEnded"));
		Montage_SetEndDelegate(EndDelegate, Chosen);
		return true;
	}
	return false;
}

void UAnimalAnimInstance::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnIdleMontageEnded.Broadcast(Montage, bInterrupted);
}