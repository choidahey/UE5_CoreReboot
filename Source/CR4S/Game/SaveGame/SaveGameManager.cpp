#include "Game/SaveGame/SaveGameManager.h"
#include "Game/SaveGame/C4MetaSaveGame.h"
#include "Game/SaveGame/WorldSaveGame.h"
#include "Game/SaveGame/SettingsSaveGame.h"
#include "Game/SaveGame/CoreSaveGame.h"
#include "Game/System/AudioManager.h"
#include "Game/GameInstance/C4GameInstance.h"
//#include "Game/SaveGame/BuildingSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void USaveGameManager::SaveAll(const FString& SlotName)
{
    //UBuildingSaveGame* Building = NewObject<UBuildingSaveGame>();
    //UGameplayStatics::SaveGameToSlot(Building, SlotName + "_Building", 0);

    SaveCore(SlotName);

    //SaveWorld(SlotName);

    if (!MetaSave)
        MetaSave = NewObject<UC4MetaSaveGame>();

    FDateTime Now = FDateTime::Now();
    FSaveSlotMetaData Data;
    Data.SlotName = SlotName;
    Data.SaveTime = Now;

    MetaSave->SaveSlots.Add(SlotName, Data);
    SaveMeta();
}

void USaveGameManager::PreloadSaveData(const FString& SlotName)
{
    // Core
    if (UGameplayStatics::DoesSaveGameExist(SlotName + "_Core", 0))
    {
        CoreSave = Cast<UCoreSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName + "_Core", 0));
        if (CoreSave)
        {
            //
        }
    }
}

void USaveGameManager::SaveMeta()
{
    if (!MetaSave)
        MetaSave = NewObject<UC4MetaSaveGame>();

    UGameplayStatics::SaveGameToSlot(MetaSave, MetaSlotName, 0);
}

void USaveGameManager::LoadMeta()
{
    if (UGameplayStatics::DoesSaveGameExist(MetaSlotName, 0))
    {
        MetaSave = Cast<UC4MetaSaveGame>(UGameplayStatics::LoadGameFromSlot(MetaSlotName, 0));
    }
    else
    {
        MetaSave = NewObject<UC4MetaSaveGame>();
    }
}

namespace
{
    const FString SettingsSlotName = TEXT("SettingsSaveData");
}


void USaveGameManager::SaveSettings()
{
    if (!SettingsSave)
    {
        SettingsSave = NewObject<USettingsSaveGame>();
    }

    if (UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>())
    {
        SettingsSave->MasterVolume = AudioManager->GetMasterVolume();
        SettingsSave->BGMVolume = AudioManager->GetBGMVolume();
        SettingsSave->SFXVolume = AudioManager->GetSFXVolume();
    }

    UGameplayStatics::SaveGameToSlot(SettingsSave, SettingsSlotName, 0);
}

USettingsSaveGame* USaveGameManager::LoadSettings()
{
    if (UGameplayStatics::DoesSaveGameExist(SettingsSlotName, 0))
    {
        SettingsSave = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSlotName, 0));

        if (SettingsSave)
        {
            if (UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>())
            {
                AudioManager->SetMasterVolume(SettingsSave->MasterVolume);
                AudioManager->SetBGMVolume(SettingsSave->BGMVolume);
                AudioManager->SetSFXVolume(SettingsSave->SFXVolume);
            }
		}

        return SettingsSave;
    }

	return NewObject<USettingsSaveGame>();
}

void USaveGameManager::DeleteSaveGame(const FString& SlotName)
{
    UGameplayStatics::DeleteGameInSlot(SlotName + "_World", 0);
    UGameplayStatics::DeleteGameInSlot(SlotName + "_Settings", 0);
    UGameplayStatics::DeleteGameInSlot(SlotName + "_Core", 0);
    if (MetaSave && MetaSave->SaveSlots.Contains(SlotName))
    {
        MetaSave->SaveSlots.Remove(SlotName);
        SaveMeta();
	}
}

FSaveSlotMetaData USaveGameManager::GetSaveMetaDataByIndex(int32 Index) const
{
    if (!MetaSave) return FSaveSlotMetaData();

    FString Key = FString::FromInt(Index);
    if (const FSaveSlotMetaData* Found = MetaSave->SaveSlots.Find(Key))
    {
        return *Found;
    }

    return FSaveSlotMetaData();
}

void USaveGameManager::CreateSlot(const FString& SlotName)
{
    if (!MetaSave)
        MetaSave = NewObject<UC4MetaSaveGame>();

    FDateTime Now = FDateTime::Now();
    FSaveSlotMetaData Data;
    Data.SlotName = SlotName;
    Data.SaveTime = Now;
    MetaSave->SaveSlots.Add(SlotName, Data);
    SaveMeta();
}

void USaveGameManager::DeleteSlot(const FString& SlotName)
{
    if (MetaSave && MetaSave->SaveSlots.Contains(SlotName))
    {
        MetaSave->SaveSlots.Remove(SlotName);

        UGameplayStatics::DeleteGameInSlot(SlotName + "_World", 0);
        UGameplayStatics::DeleteGameInSlot(SlotName + "_Settings", 0);
        UGameplayStatics::DeleteGameInSlot(SlotName + "_Core", 0);

        SaveMeta();
	}
}

void USaveGameManager::SaveCore(const FString& SlotName)
{
    UE_LOG(LogTemp, Log, TEXT("[SaveCore] Called with SlotName: %s"), *SlotName);

    CoreSave = NewObject<UCoreSaveGame>();
    if (!CoreSave)
    {
        UE_LOG(LogTemp, Error, TEXT("[SaveCore] Failed to create CoreSave object"));
        return;
    }

    if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
    {
        CoreSave->PlayerLocation = Player->GetActorLocation();
        CoreSave->PlayerRotation = Player->GetActorRotation();

        UE_LOG(LogTemp, Log, TEXT("[SaveCore] PlayerLocation: %s"), *CoreSave->PlayerLocation.ToString());
        UE_LOG(LogTemp, Log, TEXT("[SaveCore] PlayerRotation: %s"), *CoreSave->PlayerRotation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SaveCore] PlayerCharacter not found"));
    }

    const FString FullSlotName = SlotName + TEXT("_Core");
    const bool bSuccess = UGameplayStatics::SaveGameToSlot(CoreSave, FullSlotName, 0);

    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[SaveCore] Successfully saved to slot: %s"), *FullSlotName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SaveCore] Failed to save to slot: %s"), *FullSlotName);
    }
}


void USaveGameManager::SaveWorld(const FString& SlotName)
{
    WorldSave = NewObject<UWorldSaveGame>();

    if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
    {
        
    }

    UGameplayStatics::SaveGameToSlot(WorldSave, SlotName + "_World", 0);
}

bool USaveGameManager::IsNewGame() const
{
    const UC4GameInstance* GameInstance = Cast<UC4GameInstance>(GetGameInstance());
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[IsNewGame] GameInstance is null"));
        return true;
    }

    const bool bIsExistingSlot = MetaSave && MetaSave->SaveSlots.Contains(GameInstance->CurrentSlotName);

    UE_LOG(LogTemp, Log, TEXT("[IsNewGame] CurrentSlotName: %s, IsNewGame: %s"),
        *GameInstance->CurrentSlotName, bIsExistingSlot ? TEXT("false") : TEXT("true"));

    return !bIsExistingSlot;
}


void USaveGameManager::ApplyAll()
{
    if (!CoreSave )
    {
        UE_LOG(LogTemp, Warning, TEXT("[ApplyAll] Save data not loaded. Call PreloadSaveData first."));
        return;
    }

    const UC4GameInstance* GameInstance = Cast<UC4GameInstance>(GetGameInstance());


    ApplyCoreData();
    ApplyWorldData();
}

void USaveGameManager::ApplyCoreData()
{
    if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
    {
        Player->SetActorLocation(CoreSave->PlayerLocation);
        Player->SetActorRotation(CoreSave->PlayerRotation);

        UE_LOG(LogTemp, Log, TEXT("[ApplyCoreData] Player location and rotation applied"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[ApplyCoreData] PlayerCharacter not found"));
    }
}

void USaveGameManager::ApplyWorldData()
{
}