#include "CharacterController.h"
#include "Game/CheatManager/C4CheatManager.h"
#include "UI/InGame/SurvivalHUD.h"

ACharacterController::ACharacterController():
	DefaultMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr)
{
	CheatClass = UC4CheatManager::StaticClass();
}

void ACharacterController::OnPauseRequested()
{
    UE_LOG(LogTemp, Log, TEXT("HandleTogglePauseMenu() called."));

    ASurvivalHUD* InGameHUD = Cast<ASurvivalHUD>(GetHUD());
	if (InGameHUD)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully cast to ASurvivalHUD. Toggling pause menu."));
		InGameHUD->HandlePauseToggle();
	}
}
