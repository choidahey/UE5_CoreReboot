#include "CharacterController.h"

#include "EnhancedInputSubsystems.h"
#include "Game/CheatManager/C4CheatManager.h"
#include "UI/InGame/SurvivalHUD.h"

ACharacterController::ACharacterController()
{
	CheatClass = UC4CheatManager::StaticClass();
}

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LP=GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem=LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext,0);
		}
	}
	
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void ACharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Bind Actions related UI
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
