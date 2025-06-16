#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameManager.generated.h"

class UC4MetaSaveGame;
class UWorldSaveGame;
class USettingsSaveGame;

UCLASS()
class CR4S_API USaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region Getters

public:
    void SaveAll(const FString& SlotName);
    void LoadAll(const FString& SlotName);

    void SaveMeta();
    void LoadMeta();

    void SaveSettings();
	USettingsSaveGame* LoadSettings();

	void DeleteSaveGame(const FString& SlotName);

private:

    FString MetaSlotName = TEXT("MetaSlot");

    UPROPERTY()
    TObjectPtr<UC4MetaSaveGame> MetaSave;
    UPROPERTY()
	TObjectPtr<UWorldSaveGame> WorldSave;
    UPROPERTY()
	TObjectPtr<USettingsSaveGame> SettingsSave;

	
};
