#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AMTargetEvaluation.generated.h"

UCLASS()
class CR4S_API UBTService_AMTargetEvaluation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_AMTargetEvaluation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};