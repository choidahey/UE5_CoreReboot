#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TestBTT_GFMove.generated.h"

UCLASS()
class CR4S_API UTestBTT_GFMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTestBTT_GFMove();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
