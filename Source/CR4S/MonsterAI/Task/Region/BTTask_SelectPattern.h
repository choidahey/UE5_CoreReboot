#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectPattern.generated.h"

UCLASS()
class CR4S_API UBTTask_SelectPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectPattern();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FString MyHeader = TEXT("BTTask_SelectPattern");
	
};
