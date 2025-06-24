#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_IncrementPatternStep.generated.h"

UCLASS()
class CR4S_API UBTTask_IncrementPatternStep : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_IncrementPatternStep();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FString MyHeader = TEXT("BTTask_IncrementPatternStep");

};
