#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "DummyTargetActor.generated.h"

UCLASS()
class CR4S_API ADummyTargetActor : public AActor
{
	GENERATED_BODY()
   
public:	
	ADummyTargetActor();

protected:
	UPROPERTY()
	USceneComponent* RootSceneComponent;
};