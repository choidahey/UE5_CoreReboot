#include "Game/CheatManager/SaveGameHelper.h"
#include "Game/GameInstance/C4GameInstance.h"
#include "Game/SaveGame/SaveGameManager.h"

void USaveGameHelper::SaveNow()
{
    if (UWorld* World = GetWorld())
    {
        UC4GameInstance* GameInstance = Cast<UC4GameInstance>(World->GetGameInstance());
        if (GameInstance)
        {
            USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
            SaveGameManager->SaveAll(GameInstance->CurrentSlotName);
        }
    }
}

void USaveGameHelper::LoadGame()
{
    if (UWorld* World = GetWorld())
    {
        UC4GameInstance* GameInstance = Cast<UC4GameInstance>(World->GetGameInstance());
        if (GameInstance)
        {
            USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
            SaveGameManager->PreloadSaveData(GameInstance->CurrentSlotName);
            SaveGameManager->ApplyAll();
        }
    }
}
