#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Controller/BaseMonsterAIController.h"
#include "SeasonBossMonsterAIController.generated.h"

UCLASS()
class CR4S_API ASeasonBossMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "BossAI")
	FVector GetPlayerInitialLocation() const;

	UFUNCTION(BlueprintCallable, Category = "BossAI")
	FVector GetNearestHouseLocation(const FVector& PlayerLocation) const;
};
