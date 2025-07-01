#pragma once

#include "GameFramework/SaveGame.h"
#include "C4MetaSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSaveSlotMetaData
{
    GENERATED_BODY()

    UPROPERTY()
    FString SlotName; // ex: "SaveSlot_01"

    UPROPERTY()
    FDateTime SaveTime;

    UPROPERTY()
    FString ThumbnailPath;

    UPROPERTY()
    FString TotalGameTime;

    UPROPERTY()
    FString Season;

    TWeakObjectPtr<class UCoreSaveGame> CoreSaveGamePtr;
    TWeakObjectPtr<class UWorldSaveGame> WorldSaveGamePtr;

    bool IsValid() const
    {
        return !SlotName.IsEmpty();
    }
};

UCLASS()
class CR4S_API UC4MetaSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

    UPROPERTY(BlueprintReadWrite)
    TMap<FString, FSaveSlotMetaData> SaveSlots;
};
