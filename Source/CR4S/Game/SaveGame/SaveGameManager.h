#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Game/SaveGame/C4MetaSaveGame.h"
#include "SaveGameManager.generated.h"

class USpawnActorClassConfig;
class ISavableActor;
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

#pragma region Utilities
    void RegisterSavableActor(TScriptInterface<ISavableActor> SavableActor);
    void UnregisterSavableActor(TScriptInterface<ISavableActor> SavableActor);

    FName GenerateUniqueID();
#pragma endregion
    
#pragma region Public API - Save / Load
public:

    UFUNCTION(BlueprintCallable)
    void PreloadSaveData(const FString& SlotName);
 
    UFUNCTION(BlueprintCallable)
    void ApplyCoreData();
    UFUNCTION(BlueprintCallable)
    void ApplyWorldData();
    UFUNCTION(BlueprintCallable)
    void ApplyBuildingData();

    UFUNCTION(BlueprintCallable)
    bool SaveCore(const FString& SlotName);
    UFUNCTION(BlueprintCallable)
    bool SaveWorld(const FString& SlotName);
    UFUNCTION(BlueprintCallable)
    bool SaveBuilding(const FString& SlotName);

    void SaveSettings(USettingsSaveGame* InSettingsSave);


    USettingsSaveGame* LoadSettings();


    void DeleteSaveGame(const FString& SlotName);

    bool SaveAll(const FString& SlotName);
    void ApplyAll();

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
    TObjectPtr<USpawnActorClassConfig> SpawnClassDataAsset;
    
    UPROPERTY()
    TArray<TScriptInterface<ISavableActor>> SavableActors;
    
    UPROPERTY()
    TObjectPtr<UC4MetaSaveGame> MetaSave;
    UPROPERTY()
    TObjectPtr<UWorldSaveGame> WorldSave;
    UPROPERTY()
    TObjectPtr<UCoreSaveGame> CoreSave;
    UPROPERTY()
    TObjectPtr<USaveGame> BuildingSave;

    UPROPERTY()
    TObjectPtr<USettingsSaveGame> SettingsSave;

private:
    FString MetaSlotName = TEXT("MetaSlot");

#pragma endregion

};
