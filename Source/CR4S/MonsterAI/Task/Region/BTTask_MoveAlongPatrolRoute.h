#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveAlongPatrolRoute.generated.h"

UCLASS()
class CR4S_API UBTTask_MoveAlongPatrolRoute : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveAlongPatrolRoute();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
