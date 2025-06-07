#include "MonsterAI/Animations/MonsterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningPawn) return;

	ActorRotation = OwningPawn->GetActorRotation();

	if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
	{
		if (AController* Ctrl = Char->GetController())
		{
			ControlRotation = Ctrl->GetControlRotation();
		}
	}

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);
	AimYaw = DeltaRot.Yaw;
	AimPitch = DeltaRot.Pitch;
}
