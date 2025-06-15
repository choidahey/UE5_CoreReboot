#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FAFlyLand.generated.h"

UCLASS()
class CR4S_API UBTT_FAFlyLand : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FAFlyLand();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
