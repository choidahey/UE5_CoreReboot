#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "CeresRainyBossMonster.generated.h"

UCLASS()
class CR4S_API ACeresRainyBossMonster : public ASeasonBossMonster
{
	GENERATED_BODY()

public:
	ACeresRainyBossMonster();

private:
	FString MyHeader = TEXT("CeresRainyBossMonster");
};
