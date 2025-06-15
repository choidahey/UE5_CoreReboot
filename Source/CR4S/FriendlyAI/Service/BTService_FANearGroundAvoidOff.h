#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FANearGroundAvoidOff.generated.h"

UCLASS()
class CR4S_API UBTService_FANearGroundAvoidOff : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FANearGroundAvoidOff();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
