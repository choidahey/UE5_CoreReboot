#include "Game/SaveGame/SaveGameManager.h"
#include "Game/System/AudioManager.h"
#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonManager.h"

#include "Game/SaveGame/C4MetaSaveGame.h"
#include "Game/SaveGame/WorldSaveGame.h"
#include "Game/SaveGame/SettingsSaveGame.h"
#include "Game/SaveGame/CoreSaveGame.h"
//#include "Game/SaveGame/BuildingSaveGame.h"


#include "Game/GameInstance/C4GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "CR4S.h"
#include "SpawnActorClassConfig.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Game/Interface/SavableActor.h"

void USaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (const UCR4SDataTableSettings* Settings=GetDefault<UCR4SDataTableSettings>())
    {
        SpawnClassDataAsset=Cast<USpawnActorClassConfig>(Settings->GetDataAssetByName(TEXT("SpawnClassDataAsset")));
    }
}

void USaveGameManager::SaveAll(const FString& SlotName)
{
    //UBuildingSaveGame* Building = NewObject<UBuildingSaveGame>();
    //UGameplayStatics::SaveGameToSlot(Building, SlotName + "_Building", 0);

    SaveCore(SlotName);

    SaveWorld(SlotName);

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
    
    if (UGameplayStatics::DoesSaveGameExist(SlotName + "_Core", 0))
    {
        CoreSave = Cast<UCoreSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName + "_Core", 0));
    }
    if (UGameplayStatics::DoesSaveGameExist(SlotName + "_World", 0))
    {
        WorldSave = Cast<UWorldSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName + "_World", 0));
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

        return SettingsSave;
    }

	return NewObject<USettingsSaveGame>();
}

void USaveGameManager::DeleteSaveGame(const FString& SlotName)
{
    UGameplayStatics::DeleteGameInSlot(SlotName + "_World", 0);
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

void USaveGameManager::RegisterSavableActor(TScriptInterface<ISavableActor> SavableActor)
{
    if (SavableActor)
    {
        SavableActors.AddUnique(SavableActor);
    }
}

void USaveGameManager::UnregisterSavableActor(TScriptInterface<ISavableActor> SavableActor)
{
    if (SavableActor)
    {
        SavableActors.Remove(SavableActor);
    }
}

FName USaveGameManager::GenerateUniqueID()
{
    if (!CoreSave)
    {
        CoreSave=NewObject<UCoreSaveGame>();
    }

    const int32 IDNumber=CoreSave->NextUniqueID;
    CoreSave->NextUniqueID++;

    return FName(*FString::Printf(TEXT("UniqueID_%d"),IDNumber));
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
    CR4S_Log(LogSave, Log, TEXT("Called with SlotName: %s"), *SlotName);
    if (!CoreSave)
    {
        CoreSave = NewObject<UCoreSaveGame>();
    }

    CoreSave->SavedActorsData.Empty();

   for (const TScriptInterface<ISavableActor>& SavableActor : SavableActors)
   {
       if (!SavableActor) return;

       FName UniqueID=SavableActor->GetUniqueSaveID();
       if (UniqueID.IsNone())
       {
           UniqueID=GenerateUniqueID();
           SavableActor->SetUniqueSaveID(UniqueID);
       }

       FSavedActorData ActorDataContainer;

       SavableActor->GatherSaveData(ActorDataContainer);

       CoreSave->SavedActorsData.Add(UniqueID, ActorDataContainer);
   }

    
    const FString FullSlotName = SlotName + TEXT("_Core");
    const bool bSuccess = UGameplayStatics::SaveGameToSlot(CoreSave, FullSlotName, 0);
    
    if (bSuccess)
    {
        CR4S_Log(LogSave, Log, TEXT("Successfully saved to slot: %s"), *FullSlotName);
    }
    else
    {
        CR4S_Log(LogSave, Error, TEXT("Failed to save to slot: %s"), *FullSlotName);
    }
}


void USaveGameManager::SaveWorld(const FString& SlotName)
{
    WorldSave = NewObject<UWorldSaveGame>();
    if (!CR4S_VALIDATE(LogSave, WorldSave)) return;

    UWorldTimeManager* TimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
    if (!CR4S_VALIDATE(LogSave, TimeManager)) return;

    USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
    if (!CR4S_VALIDATE(LogSave, SeasonManager)) return;

    WorldSave->Season = SeasonManager->GetCurrentSeason();
    WorldSave->SeasonDay = SeasonManager->GetCurrentSeasonDay();
    WorldSave->DawnTime = SeasonManager->GetCurrentDawnTime();
    WorldSave->DuskTime = SeasonManager->GetCurrentDuskTime();
    WorldSave->SeasonLength = SeasonManager->GetSeasonLength();

    CR4S_Log(LogSave, Log, TEXT("Season data saved: [%s] Day %d, Dawn %.2f, Dusk %.2f, Length %d"),
        *UEnum::GetValueAsString(WorldSave->Season),
        WorldSave->SeasonDay,
        WorldSave->DawnTime,
        WorldSave->DuskTime,
        WorldSave->SeasonLength);


    WorldSave->TimeData = TimeManager->GetCurrentTimeData();
    WorldSave->TotalGameTime = TimeManager->GetTotalPlayTime();
    WorldSave->DayCycleLength = TimeManager->GetDayCycleLength();

    CR4S_Log(LogSave, Log, TEXT("Time data saved: Current=%s, TotalTime=%lld, CycleLength=%d"),
        *WorldSave->TimeData.ToString(), 
        WorldSave->TotalGameTime,
        WorldSave->DayCycleLength);

    const FString FullSlotName = SlotName + TEXT("_World");
    const bool bSuccess = UGameplayStatics::SaveGameToSlot(WorldSave, FullSlotName, 0);

    if (bSuccess)
    {
        CR4S_Log(LogSave, Log, TEXT("WorldSave successfully saved to slot: %s"), *FullSlotName);
    }
    else
    {
        CR4S_Log(LogSave, Error, TEXT("Failed to save WorldSave to slot: %s"), *FullSlotName);
    }
}


bool USaveGameManager::IsNewGame() const
{
    CR4S_SIMPLE_SCOPE_LOG;
    const UC4GameInstance* GameInstance = Cast<UC4GameInstance>(GetGameInstance());
    if (!GameInstance)
    {
        CR4S_Log(LogSave, Warning, TEXT("GameInstance is null"));
        return true;
    }

    const bool bIsExistingSlot = MetaSave && MetaSave->SaveSlots.Contains(GameInstance->CurrentSlotName);
    const FString SlotName = GameInstance->CurrentSlotName;
    const bool bHasCore = UGameplayStatics::DoesSaveGameExist(SlotName + TEXT("_Core"), 0);
    const bool bHasWorld = UGameplayStatics::DoesSaveGameExist(SlotName + TEXT("_World"), 0);
    const bool bIsNew = !(bHasCore && bHasWorld);

    CR4S_Log(LogSave, Log, TEXT("SlotName: %s, HasCore: %s, HasWorld: %s, IsNewGame: %s"),
        *SlotName,
        bHasCore ? TEXT("true") : TEXT("false"),
        bHasWorld ? TEXT("true") : TEXT("false"),
        bIsNew ? TEXT("true") : TEXT("false"));

    return bIsNew;
}


void USaveGameManager::ApplyAll()
{
    CR4S_SIMPLE_SCOPE_LOG;
    if (!CR4S_VALIDATE(LogSave, CoreSave)) return;
    if (!CR4S_VALIDATE(LogSave, WorldSave)) return;

    ApplyCoreData();
    ApplyWorldData();
}

void USaveGameManager::ApplyCoreData()
{
    if (!CR4S_ENSURE(LogHong1,CoreSave && SpawnClassDataAsset)) return;
    
    for (int32 i=SavableActors.Num()-1;i>=0;i--)
    {
        if (AActor* ActorToDestroy=Cast<AActor>(SavableActors[i].GetObject()))
        {
            ActorToDestroy->Destroy();
        }
    }

    SavableActors.Empty();

    for (auto& Pair : CoreSave->SavedActorsData)
    {
        const FName& ActorID=Pair.Key;
        FSavedActorData& SavedData = Pair.Value;

        UClass* ActorClassToSpawn = nullptr;
        AActor* NewSpawnedActor = nullptr;
        
        switch (SavedData.ActorType)
        {
        case ESavedActorType::PlayerCharacter:
            ActorClassToSpawn = SpawnClassDataAsset->PlayerCharacterClass;
            break;
        case ESavedActorType::ModularRobot:
            ActorClassToSpawn = SpawnClassDataAsset->ModularRobotClass;
            break;
        default:
            continue; // 타입이 없으면 건너뛰기
        }

        if (ActorClassToSpawn)
        {
            UE_LOG(LogHong1, Log, TEXT("Spawn Actor: %s"), *ActorClassToSpawn->GetName());
            NewSpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClassToSpawn);
        }

        if (NewSpawnedActor)
        {
            if (ISavableActor* Savable = Cast<ISavableActor>(NewSpawnedActor))
            {
                Savable->SetUniqueSaveID(ActorID);
                Savable->ApplySaveData(SavedData);

                if (APlayerCharacter* NewPlayer = Cast<APlayerCharacter>(NewSpawnedActor))
                {
                    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
                    {
                        UE_LOG(LogHong1, Log, TEXT("Possess Player"));
                        PC->Possess(NewPlayer);
                    }
                }
            }
        }
    }
    
}

void USaveGameManager::ApplyWorldData()
{
    UWorldTimeManager* TimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
    if (!CR4S_VALIDATE(LogSave, TimeManager)) return;

    TimeManager->SetDayCycleLength(WorldSave->DayCycleLength);
    TimeManager->SetWorldTime(WorldSave->TimeData);
 /*   TimeManager->SetTotalPlayTime(WorldSave->TotalGameTime);*/

    USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
    if (!CR4S_VALIDATE(LogSave, SeasonManager)) return;

    SeasonManager->SetSeasonLength(WorldSave->SeasonLength);
    SeasonManager->SetCurrentSeasonDay(WorldSave->SeasonDay);
    SeasonManager->SetCurrentDawnDuskTime(WorldSave->DawnTime, WorldSave->DuskTime);
    SeasonManager->SetCurrentSeason(WorldSave->Season);

    TimeManager->UpdateTimeWidget();

    CR4S_Log(LogSave, Log, TEXT(
        "Applied World Data:\n"
        "  DayCycleLength: %d\n"
        "  TimeData: %s\n"
        "  TotalGameTime: %lld\n"
        "  SeasonLength: %d\n"
        "  SeasonDay: %d\n"
        "  DawnTime: %.2f\n"
        "  DuskTime: %.2f\n"
        "  Season: %s"
    ),
        WorldSave->DayCycleLength,
        *WorldSave->TimeData.ToString(),
        WorldSave->TotalGameTime,
        WorldSave->SeasonLength,
        WorldSave->SeasonDay,
        WorldSave->DawnTime,
        WorldSave->DuskTime,
        *UEnum::GetValueAsString(WorldSave->Season)
    );
}
