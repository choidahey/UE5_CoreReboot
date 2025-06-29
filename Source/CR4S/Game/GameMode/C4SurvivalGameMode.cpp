#include "Game/GameMode/C4SurvivalGameMode.h"
#include "Game/SaveGame/SaveGameManager.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Kismet/GameplayStatics.h"


void AC4SurvivalGameMode::StartPlay()
{
    Super::StartPlay();

    UE_LOG(LogTemp, Warning, TEXT("StartPlay time: %.6f"), FPlatformTime::Seconds());

    HandleStartGame();
}

void AC4SurvivalGameMode::ReturnToMenuWithDelay(float Delay)
{
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC4SurvivalGameMode::ReturnToMenu, Delay, false);
}

void AC4SurvivalGameMode::ReturnToMenu()
{
	UGameplayStatics::OpenLevel(this, FName("MenuLevel"));
}

void AC4SurvivalGameMode::EndGameSequence()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) return;

    ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD());
    if (HUD)
    {
        HUD->PlayEndingSequence();
    }
}

void AC4SurvivalGameMode::HandleStartGame()
{
    USaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
    if (SaveGameManager->IsNewGame())
    {
        // Add New Game Logic
    }

    else
    {
        SaveGameManager->ApplyAll();

        if (GameStartSFX)
        {
            UGameplayStatics::PlaySound2D(this, GameStartSFX);
        }
    }
}