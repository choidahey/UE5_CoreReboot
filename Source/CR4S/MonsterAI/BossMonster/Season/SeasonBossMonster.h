#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BaseMonster.h"
#include "SeasonBossMonster.generated.h"


UCLASS()
class CR4S_API ASeasonBossMonster : public ABaseMonster
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
