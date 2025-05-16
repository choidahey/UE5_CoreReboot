#include "BaseHelperBot.h"

ABaseHelperBot::ABaseHelperBot()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseHelperBot::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseHelperBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseHelperBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

