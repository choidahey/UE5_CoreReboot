#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Game/SaveGame/C4MetaSaveGame.h"
#include "SaveGameManager.generated.h"

class UC4MetaSaveGame;
class UWorldSaveGame;
class USettingsSaveGame;
class UCoreSaveGame;

UCLASS()
class CR4S_API USaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
#pragma region Getters and Setters
public:
    FORCEINLINE TObjectPtr<UC4MetaSaveGame> GetMetaSave() const { return MetaSave; }

    FSaveSlotMetaData GetSaveMetaDataByIndex(int32 Index) const;

#pragma endregion


#pragma region Public API - Save / Load
public:
    void SaveAll(const FString& SlotName);
    void PreloadSaveData(const FString& SlotName);
    void ApplyAll();
    void ApplyCoreData();
    void ApplyWorldData();

    void SaveCore(const FString& SlotName);
    void SaveWorld(const FString& SlotName);

    void SaveSettings();
    USettingsSaveGame* LoadSettings();

    void DeleteSaveGame(const FString& SlotName);

#pragma endregion

#pragma region Public API - Slot Management
public:

    void CreateSlot(const FString& SlotName);
    void DeleteSlot(const FString& SlotName);

    void SaveMeta();
    void LoadMeta();

    bool IsNewGame() const;
#pragma endregion

#pragma region Internal State

protected:

    UPROPERTY()
    TObjectPtr<UC4MetaSaveGame> MetaSave;
    UPROPERTY()
    TObjectPtr<UWorldSaveGame> WorldSave;
    UPROPERTY()
    TObjectPtr<UCoreSaveGame> CoreSave;
    UPROPERTY()
    TObjectPtr<USettingsSaveGame> SettingsSave;

private:
    FString MetaSlotName = TEXT("MetaSlot");

#pragma endregion

};
