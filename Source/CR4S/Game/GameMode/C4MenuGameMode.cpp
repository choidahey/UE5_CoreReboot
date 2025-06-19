#include "Game/GameMode/C4MenuGameMode.h"
#include "Game/Controller/MenuPlayerController.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"


void AC4MenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			AMenuPlayerController* MenuPC = Cast<AMenuPlayerController>(PC);
            if (MenuPC)
            {
                MenuPC->bShowMouseCursor = true;
                MenuPC->SetInputMode(FInputModeUIOnly());
            }
        }
    }
}

void AC4MenuGameMode::OpenSurvivalLevel(int32 SlotIndex)
{
    UGameplayStatics::OpenLevel(this, FName("SurvivalLevel_1"));
}