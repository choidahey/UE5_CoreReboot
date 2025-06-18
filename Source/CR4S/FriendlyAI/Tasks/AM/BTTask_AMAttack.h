#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AMAttack.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None,
	Melee,
	Charge,
	Ranged
};

UCLASS()
class CR4S_API UBTTask_AMAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AMAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	EAttackType SelectAttackType(class AAnimalMonster* Monster, float DistanceToTarget);
	TArray<EAttackType> GetAvailableAttacks(class AAnimalMonster* Monster, float DistanceToTarget);
	bool CanPerformAttack(class AAnimalMonster* Monster, EAttackType AttackType, float DistanceToTarget);
	void ExecuteAttack(class AAnimalMonster* Monster, EAttackType AttackType);
};