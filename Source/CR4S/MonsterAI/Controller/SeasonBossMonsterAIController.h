#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"
#include "SeasonBossMonsterAIController.generated.h"

UCLASS()
class CR4S_API ASeasonBossMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()

public:
	ASeasonBossMonsterAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION()
	void HandleStunStarted();

	UFUNCTION()
	void HandleStunEnded();

	TWeakObjectPtr<AActor> LastFocusedTarget;
};
