#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Game/System/SeasonType.h"
#include "SeasonWave.generated.h"

USTRUCT(BlueprintType)
struct FMonsterWaveEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bIsBoss = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SpawnCount = 1;
};

USTRUCT(BlueprintType)
struct FMonsterWaveArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FMonsterWaveEntry> Entries;
};

UCLASS()
class CR4S_API USeasonWave : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<ESeasonType, FMonsterWaveArray> SeasonWaves;
};
