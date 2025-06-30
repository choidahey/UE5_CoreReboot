#include "Game/GameMode/C4MenuGameMode.h"
#include "Game/GameInstance/C4GameInstance.h"
#include "Game/Controller/MenuPlayerController.h"
#include "Game/System/AudioManager.h"
#include "Game/SaveGame/SaveGameManager.h"
#include "UI/MainMenu/MainMenuWidget.h"
#include "UI/Common/LoadingWidget.h"
#include "Kismet/GameplayStatics.h"


void AC4MenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuWidgetClass)
    {
        MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();
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
    UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();
    AudioManager->StopBGM();
    
    UC4GameInstance* GameInstance = Cast<UC4GameInstance>(GetGameInstance());
    GameInstance->CurrentSlotName = FString::FromInt(SlotIndex);

    USaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
    SaveGameManager->PreloadSaveData(FString::FromInt(SlotIndex));

    if(LoadingWidgetClass)
    {
        ULoadingWidget* LoadingWidget = CreateWidget<ULoadingWidget>(GetWorld(), LoadingWidgetClass);
        if (LoadingWidget)
        {
            LoadingWidget->AddToViewport();
        }
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AC4MenuGameMode::LoadInGameLevel, 1.0f, false);

}

void AC4MenuGameMode::LoadInGameLevel()
{
    UGameplayStatics::OpenLevel(this, FName("InGameMap"));
}