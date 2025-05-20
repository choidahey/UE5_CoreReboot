#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CR4S/MonsterAI/Data/MonsterEnum.h"
#include "BTTask_SetMonsterState.generated.h"

UCLASS()
class CR4S_API UBTTask_SetMonsterState : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetMonsterState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector StateKey;

};
