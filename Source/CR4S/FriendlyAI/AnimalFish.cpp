#include "AnimalFish.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AAnimalFish::AAnimalFish()
{
	PrimaryActorTick.bCanEverTick = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DefaultLandMovementMode = MOVE_Swimming;
		Movement->DefaultWaterMovementMode = MOVE_Swimming;
		Movement->SetMovementMode(MOVE_Swimming);
		Movement->bCanWalkOffLedges = false;
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		Movement->GravityScale = 0.0f;
	}

	bCanMelee = false;
	bCanCharge = false;
	bCanRanged = false;
}

void AAnimalFish::BeginPlay()
{
	Super::BeginPlay();
	
	LastValidWaterPosition = GetActorLocation();
}

bool AAnimalFish::IsInWater() const
{
	return true;
}