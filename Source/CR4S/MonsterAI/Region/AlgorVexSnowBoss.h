#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "AlgorVexSnowBoss.generated.h"

UCLASS()
class CR4S_API AAlgorVexSnowBoss : public ARegionBossMonster
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Buff")
	bool bIsIceClawBuffActive = false;
	
};
