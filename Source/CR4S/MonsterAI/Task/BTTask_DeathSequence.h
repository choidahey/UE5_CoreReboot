#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DeathSequence.generated.h"

UCLASS()
class CR4S_API UBTTask_DeathSequence : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_DeathSequence();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
