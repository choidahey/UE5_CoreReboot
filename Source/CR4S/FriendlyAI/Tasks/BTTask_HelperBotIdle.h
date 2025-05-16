#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HelperBotIdle.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperBotIdle : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotIdle();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};