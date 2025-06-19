#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HelperBotAttack.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperBotAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotAttack();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange = 0.0f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UFUNCTION() 
	void OnTargetDestroyed(AActor* DestroyedActor);
	
	AActor* FindNearestMonster(APawn* Helper);
	bool IsValidMonsterTarget(AActor* Actor);

	UPROPERTY()
	AActor* CachedTarget = nullptr;
	
	UPROPERTY()
	APawn* CachedHelper = nullptr;
	
	float CachedAttackPerSecond = 0.f;
};