#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToInitializeLocation.generated.h"

UCLASS()
class CR4S_API UBTTask_MoveToInitializeLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveToInitializeLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	FAIRequestID MoveRequestID;
};
