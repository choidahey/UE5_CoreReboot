#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Data/AnimalStatsRow.h"
#include "FriendlyAI/Service/BTService_AMPathCheck.h"
#include "AnimalMonsterAIController.generated.h"

class AAnimalMonster;

UCLASS()
class CR4S_API AAnimalMonsterAIController : public AAIController
{
	GENERATED_BODY()

#pragma region AAIController Override
public:
	AAnimalMonsterAIController();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
#pragma endregion

#pragma region State Management
public:
	UFUNCTION(BlueprintCallable)
	void SetMonsterState(EAnimalState NewState);
#pragma endregion

#pragma region Target Management
public:
	UFUNCTION(BlueprintCallable)
	void EvaluateTargetPriority();

	UFUNCTION(BlueprintCallable)
	void HandlePlayerPathCheck();

	UFUNCTION(BlueprintCallable)
	void HandleAutomatedAIAttack(AActor* AttackingAI);

	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void ClearTargetActor();
#pragma endregion

#pragma region Path Management
public:
	UFUNCTION(BlueprintCallable)
	bool CheckPathToPlayer();

	UFUNCTION(BlueprintCallable)
	void FindBuildingToDestroy();
#pragma endregion

#pragma region Event Handlers
public:
	UFUNCTION(BlueprintCallable)
	void OnBuildingDestroyed();

	UFUNCTION(BlueprintCallable)
	void OnTargetReached();

	UFUNCTION(BlueprintCallable)
	void OnPathBlocked();

	UFUNCTION(BlueprintCallable)
	void OnTargetOutOfRange();
#pragma endregion

#pragma region AI Components
protected:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* MonsterBehaviorTree;
#pragma endregion

#pragma region Internal Functions
private:
	void UpdateBlackboardValues();
	void HandleTargetPriorityLogic();
	bool CanReachTarget(AActor* Target) const;
	AActor* FindNearestAttackingAI() const;
	void SwitchToTarget(AActor* NewTarget);
#pragma endregion

#pragma region Cached Data
private:
	UPROPERTY()
	AAnimalMonster* ControlledMonster = nullptr;

	bool bLastCanReachPlayer = false;
	AActor* LastPrimaryTarget = nullptr;
#pragma endregion

public:
	UFUNCTION()
	bool CheckPathToTarget(AActor* Target) { return CanReachTarget(Target);	}

	void CheckDistanceAndUpdateState();
};