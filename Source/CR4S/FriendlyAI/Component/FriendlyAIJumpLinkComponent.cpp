#include "FriendlyAIJumpLinkComponent.h"
#include "GameFramework/Character.h"
#include "NavLinkCustomComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UFriendlyAIJumpLinkComponent::UFriendlyAIJumpLinkComponent()
{
	JumpHeight = 300.f;
	SetEnabled(true);
}


void UFriendlyAIJumpLinkComponent::OnRegister()
{
	Super::OnRegister();
	SetMoveReachedLink(this, &UFriendlyAIJumpLinkComponent::OnSmartLinkReached);
}

void UFriendlyAIJumpLinkComponent::OnSmartLinkReached(UNavLinkCustomComponent* ThisComp, UObject* PathComp, const FVector& DestPoint)
{
	if (AActor* OwnerActor = ThisComp->GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
		{
			Character->LaunchCharacter(FVector(0.f, 0.f, JumpHeight), false, true);
		}
	}
}
