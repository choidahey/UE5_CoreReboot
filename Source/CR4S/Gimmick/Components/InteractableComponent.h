#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override
public:
	UInteractableComponent();

#pragma endregion

#pragma region UInteractableComponent Interact

	bool TryInteract(APlayerController* PlayerController);
	
#pragma endregion
};
