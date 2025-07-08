#include "DummyTargetActor.h"

ADummyTargetActor::ADummyTargetActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
}