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
    FString Description; // ex: "Day 5, Location: Forest"

    UPROPERTY()
    FString ThumbnailPath;
};

UCLASS()
class CR4S_API UC4MetaSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

    UPROPERTY(BlueprintReadWrite)
    TMap<FString, FSaveSlotMetaData> SaveSlots;
};
