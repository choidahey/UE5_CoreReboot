#include "Game/GameMode/C4SurvivalGameMode.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Kismet/GameplayStatics.h"

void AC4SurvivalGameMode::StartPlay()
{
    Super::StartPlay();

    if (GameStartSFX)
    {
        UGameplayStatics::PlaySound2D(this, GameStartSFX);
    }
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