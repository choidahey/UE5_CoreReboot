#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FAObstacleAvoid.generated.h"

UCLASS()
class CR4S_API UBTService_FAObstacleAvoid : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FAObstacleAvoid();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
