#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Controller/SeasonBossMonsterAIController.h"
#include "FinalBossMonsterAIController.generated.h"

UCLASS()
class CR4S_API AFinalBossMonsterAIController : public ASeasonBossMonsterAIController
{
	GENERATED_BODY()
	
public:
	AFinalBossMonsterAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
