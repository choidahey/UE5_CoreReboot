#pragma once

#include "Engine/DataAsset.h"
#include "Game/System/SeasonType.h"
#include "SeasonBossDataAsset.generated.h"

class ASeasonBossMonster;

USTRUCT(BlueprintType)
struct FSeasonBossEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<ASeasonBossMonster> SeasonBossClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ESeasonType SeasonName;
};

UCLASS()
class CR4S_API USeasonBossDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FSeasonBossEntry> Bosses;

};
