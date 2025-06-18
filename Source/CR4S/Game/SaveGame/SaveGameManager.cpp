#include "Game/SaveGame/SaveGameManager.h"
#include "Game/SaveGame/C4MetaSaveGame.h"
#include "Game/SaveGame/WorldSaveGame.h"
#include "Game/SaveGame/SettingsSaveGame.h"
#include "Game/System/AudioManager.h"
//#include "Game/SaveGame/CoreSaveGame.h"
//#include "Game/SaveGame/BuildingSaveGame.h"
#include "Kismet/GameplayStatics.h"

void USaveGameManager::SaveAll(const FString& SlotName)
{
    //UCoreSaveGame* Core = NewObject<UCoreSaveGame>();
    //UGameplayStatics::SaveGameToSlot(Core, SlotName + "_Core", 0);

    //UBuildingSaveGame* Building = NewObject<UBuildingSaveGame>();
    //UGameplayStatics::SaveGameToSlot(Building, SlotName + "_Building", 0);

    WorldSave = NewObject<UWorldSaveGame>();
    UGameplayStatics::SaveGameToSlot(WorldSave, SlotName + "_World", 0);

    if (!MetaSave)
        MetaSave = NewObject<UC4MetaSaveGame>();

    FDateTime Now = FDateTime::Now();
    FSaveSlotMetaData Data;
    Data.SlotName = SlotName;
    Data.SaveTime = Now;

    MetaSave->SaveSlots.Add(SlotName, Data);
    SaveMeta();
}

void USaveGameManager::LoadAll(const FString& SlotName)
{
    // World
    if (UGameplayStatics::DoesSaveGameExist(SlotName + "_World", 0))
    {
        WorldSave = Cast<UWorldSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName + "_World", 0));
        if (WorldSave)
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
    if (MetaSave && MetaSave->SaveSlots.Contains(SlotName))
    {
        MetaSave->SaveSlots.Remove(SlotName);
        SaveMeta();
	}
}

FSaveSlotMetaData USaveGameManager::GetSaveMetaDataByIndex(int32 Index) const
{
    
    if (MetaSave && MetaSave->SaveSlots.Num() > Index)
    {
        TArray<FString> Keys;
        MetaSave->SaveSlots.GetKeys(Keys);
        if (Keys.IsValidIndex(Index))
        {
            return MetaSave->SaveSlots[Keys[Index]];
        }
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