#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"
#include "RegionBossMonsterAIController.generated.h"

UCLASS()
class CR4S_API ARegionBossMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()
	
#pragma region Constructor

public:
	ARegionBossMonsterAIController();

#pragma endregion

#pragma region Overrides
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnTargetDetected(AActor* DetectedActor) override;
	virtual void OnTargetLost(AActor* LostActor) override;

#pragma endregion

private:
	FString MyHeader;

};
