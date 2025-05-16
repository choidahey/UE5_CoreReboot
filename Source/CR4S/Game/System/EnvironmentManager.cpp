#include "Game/System/EnvironmentManager.h"

AEnvironmentManager::AEnvironmentManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnvironmentManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnvironmentManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

