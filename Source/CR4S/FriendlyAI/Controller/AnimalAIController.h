#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "../Data/AnimalStatsRow.h"
#include "AnimalAIController.generated.h"

UCLASS()
class CR4S_API AAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAnimalAIController();

	virtual void Tick(float DeltaSeconds) override;
	
	void ApplyPerceptionStats(const FAnimalStatsRow& Stats);

	UBehaviorTree* GetBehaviorTreeAsset() const { return BehaviorTreeAsset; }

	UFUNCTION(BlueprintCallable)
	void SetAnimalState(EAnimalState NewState);

	const FAnimalStatsRow& GetCurrentStats() const { return CurrentStats; }
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	void HandlePerceptionResponse(class ABaseAnimal* Animal);
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

protected:
	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

private:
	FAnimalStatsRow CurrentStats;
};