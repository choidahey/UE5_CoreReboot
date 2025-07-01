#include "Game/GameMode/C4SurvivalGameMode.h"
#include "Game/SaveGame/SaveGameManager.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "CR4S.h"

void AC4SurvivalGameMode::StartPlay()
{
    Super::StartPlay();

    CR4S_Log(LogGM, Log, TEXT("StartPlay called at %.6f seconds."), FPlatformTime::Seconds());

    HandleStartGame();
}

void AC4SurvivalGameMode::ReturnToMenuWithDelay(float Delay)
{
    CR4S_Log(LogGM, Log, TEXT("ReturnToMenuWithDelay called. Returning to MenuLevel in %.2f seconds."), Delay);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC4SurvivalGameMode::ReturnToMenu, Delay, false);
}

void AC4SurvivalGameMode::ReturnToMenu()
{
    CR4S_Log(LogGM, Log, TEXT("Opening MenuLevel..."));
    UGameplayStatics::OpenLevel(this, FName("MenuLevel"));
}

void AC4SurvivalGameMode::EndGameSequence()
{
    CR4S_SIMPLE_SCOPE_LOG;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        CR4S_Log(LogGM, Warning, TEXT("PlayerController not found."));
        return;
    }

    ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD());
    if (HUD)
    {
        CR4S_Log(LogGM, Log, TEXT("Playing ending sequence..."));
        HUD->PlayEndingSequence();
    }
    else
    {
        CR4S_Log(LogGM, Warning, TEXT("HUD cast failed."));
    }
}

void AC4SurvivalGameMode::HandleStartGame()
{
    CR4S_SIMPLE_SCOPE_LOG;

    USaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
    if (!SaveGameManager)
    {
        CR4S_Log(LogGM, Error, TEXT("SaveGameManager is null."));
        return;
    }

    if (SaveGameManager->IsNewGame())
    {
        CR4S_Log(LogGM, Log, TEXT("New game detected. Loading CinematicLevel..."));

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.ExecutionFunction = FName("OnCinematicLevelLoaded");
        LatentInfo.Linkage = 0;
        LatentInfo.UUID = __LINE__;

        UGameplayStatics::LoadStreamLevel(this, FName("_CinematicMap"), true, true, LatentInfo);
    }
    else
    {
        CR4S_Log(LogGM, Log, TEXT("Existing save found. Applying save data and starting game."));

        SaveGameManager->ApplyAll();

        if (GameStartSFX)
        {
            CR4S_Log(LogGM, Log, TEXT("Playing game start sound effect."));
            UGameplayStatics::PlaySound2D(this, GameStartSFX);
        }
    }
}

void AC4SurvivalGameMode::OnCinematicLevelLoaded()
{
    CR4S_SIMPLE_SCOPE_LOG;

    if (bIsTransitioningLevel)
    {
        CR4S_Log(LogGM, Warning, TEXT("Level is already transitioning."));
        return;
    }
    bIsTransitioningLevel = true;

    ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(this, FName("_CinematicMap"));
    if (!StreamingLevel || !StreamingLevel->IsLevelLoaded())
    {
        CR4S_Log(LogGM, Warning, TEXT("StreamingLevel is null or not loaded."));
        bIsTransitioningLevel = false;
        return;
    }

    ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();
    ALevelScriptActor* LevelScript = LoadedLevel ? LoadedLevel->GetLevelScriptActor() : nullptr;

    if (LevelScript)
    {
        if (UFunction* CustomEvent = LevelScript->FindFunction(FName("OnLevelLoaded")))
        {
            LevelScript->ProcessEvent(CustomEvent, nullptr);
        }
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
    {
        PC->SetCinematicMode(true, false, false, true, true);

        if (APawn* Pawn = PC->GetPawn())
        {
            Pawn->Destroy();
        }

        if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
        {
            HUD->HideHUD();
        }
    }

    if (!UGameplayStatics::GetStreamingLevel(this, FName("MainMap")))
    {
        CR4S_Log(LogGM, Log, TEXT("MainMap not loaded. Loading in background."));
        UGameplayStatics::LoadStreamLevel(this, FName("MainMap"), false, false, FLatentActionInfo());
    }
}


void AC4SurvivalGameMode::HandleStartNewGame()
{
    CR4S_Log(LogGM, Log, TEXT("Unloading CinematicLevel and loading MainMap..."));

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = FName("OnCinematicLevelUnloaded");
    LatentInfo.Linkage = 0;
    LatentInfo.UUID = __LINE__;

    UGameplayStatics::UnloadStreamLevel(this, FName("_CinematicMap"), LatentInfo, true);
}

void AC4SurvivalGameMode::OnCinematicLevelUnloaded()
{
    CR4S_Log(LogGM, Log, TEXT("CinematicLevel unloaded, now loading MainMap"));
    UGameplayStatics::LoadStreamLevel(this, FName("MainMap"), true, false, FLatentActionInfo());
}

void AC4SurvivalGameMode::NotifyMainMapReady()
{
    bIsTransitioningLevel = false;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC)
    {
        CR4S_Log(LogGM, Warning, TEXT("NotifyMainMapReady: PlayerController not found."));
        return;
    }

    AActor* StartSpot = FindPlayerStart(PC);
    if (!StartSpot)
    {
        CR4S_Log(LogGM, Warning, TEXT("NotifyMainMapReady: PlayerStart not found."));
        return;
    }

    FVector SpawnLoc = StartSpot->GetActorLocation();
    FRotator SpawnRot = StartSpot->GetActorRotation();

    CR4S_Log(LogGM, Log, TEXT("NotifyMainMapReady: PlayerStart found: %s"), *StartSpot->GetName());
    CR4S_Log(LogGM, Log, TEXT("NotifyMainMapReady: Spawn Location = %s, Rotation = %s"),
        *SpawnLoc.ToString(), *SpawnRot.ToString());

    RestartPlayerAtPlayerStart(PC, StartSpot);

    if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
    {
        HUD->OnlyShowDefaults();
        CR4S_Log(LogGM, Log, TEXT("NotifyMainMapReady: HUD switched to default view."));
    }
    else
    {
        CR4S_Log(LogGM, Warning, TEXT("NotifyMainMapReady: HUD not found or cast failed."));
    }
}


//void AC4SurvivalGameMode::NotifyMainMapReady()
//{
//    bIsTransitioningLevel = false;
//    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
//    if (!PC) return;
//
//    AActor* StartSpot = FindPlayerStart(PC);
//    if (!StartSpot)
//    {
//        return;
//    }
//
//    CR4S_Log(LogGM, Log, TEXT("PlayerStart found. Restarting player."));
//    RestartPlayerAtPlayerStart(PC, StartSpot);
//
//    if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
//    {
//        HUD->OnlyShowDefaults();
//    }
//}