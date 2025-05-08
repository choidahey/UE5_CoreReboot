#include "InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UInteractableComponent::TryInteract(APlayerController* PlayerController)
{
	return true;
}

