#include "JumpNavLinkComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

bool UJumpNavLinkComponent::OnLinkMoveStarted(UObject* PathComp, const FVector& DestPoint)
 {
	if (UPathFollowingComponent* PFComp = Cast<UPathFollowingComponent>(PathComp))
	{
		AController* Controller = Cast<AController>(PFComp->GetOwner());
		APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
		ACharacter* Char = Cast<ACharacter>(Pawn);
		if (Char)
		{
			UCapsuleComponent* Capsule = Char->GetCapsuleComponent();
			FVector BaseStart = Capsule
				? Capsule->GetComponentLocation() - FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight())
				: Char->GetActorLocation();
			FVector Start = BaseStart - Char->GetActorForwardVector() * 1.5;
			const float Gravity = FMath::Abs(GetWorld()->GetGravityZ());
			const float DeltaZ  = DestPoint.Z - Start.Z;
			
			if (DeltaZ <= 0.f)
			{
				if (GetOwner())
					GetOwner()->Destroy();
				return false;
			}
			
			const float RequiredVz = FMath::Sqrt(2.f * Gravity * DeltaZ);
			const float FlightTime = (2.f * RequiredVz) / Gravity;
			const FVector Dir2D    = (DestPoint - Start).GetSafeNormal2D();
			
			float HorSpeed = 0.f;
			if (FlightTime > KINDA_SMALL_NUMBER)
				HorSpeed = FVector2D(DestPoint - Start).Size() / FlightTime;
			
			float OffsetZ      = (DeltaZ > 0.f) ? JumpOffsetUp : JumpOffsetDown;
			float AdjustedVz   = RequiredVz + OffsetZ;

			FVector LaunchVel  = Dir2D * HorSpeed;
			LaunchVel.Z        = AdjustedVz;
			Char->LaunchCharacter(LaunchVel, true, true);

			if (GetOwner())
				GetOwner()->Destroy();

			// DrawDebugCapsule(
			// 	GetWorld(),
			// 	DestPoint,
			// 	10.f, 5.f,
			// 	FQuat::Identity,
			// 	FColor::Green,
			// 	false,
			// 	1.f
			// );
		}
	}

	return true;
}

void UJumpNavLinkComponent::SetRelativeStartAndEnd(const FVector& Left, const FVector& Right, AActor* Proxy)
{
	if (!Proxy) return;
	LinkRelativeStart = Proxy->GetTransform().InverseTransformPosition(Left);
	LinkRelativeEnd   = Proxy->GetTransform().InverseTransformPosition(Right);
}
