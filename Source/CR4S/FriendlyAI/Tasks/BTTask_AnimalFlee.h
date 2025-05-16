#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalFlee.generated.h"

UCLASS()
class CR4S_API UBTTask_AnimalFlee : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalFlee();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY()
	AActor* TargetToFlee;
	
};