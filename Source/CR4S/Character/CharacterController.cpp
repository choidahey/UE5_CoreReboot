#include "CharacterController.h"
#include "Game/CheatManager/C4CheatManager.h"

ACharacterController::ACharacterController():
	MenuAction(nullptr)
{
	CheatClass = UC4CheatManager::StaticClass();
}

void ACharacterController::BeginPlay()
{
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
	Super::BeginPlay();
}

void ACharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Bind Actions related UI
}
