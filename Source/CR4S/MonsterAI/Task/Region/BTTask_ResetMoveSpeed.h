#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ResetMoveSpeed.generated.h"

UCLASS()
class CR4S_API UBTTask_ResetMoveSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ResetMoveSpeed();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
