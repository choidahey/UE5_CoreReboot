#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_HelperBotAttack.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperBotAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange = 500.0f;

private:
	UPROPERTY()
	AActor* CachedTarget = nullptr;

	UPROPERTY()
	APawn* CachedHelper = nullptr;
	
	float CachedDamagePerSecond = 0.f;
	float AttackTimer = 0.f;
	const float MaxAttackDuration = 5.0f;
	
	AActor* FindNearestEnemy(APawn* Helper);
	void CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result);
};