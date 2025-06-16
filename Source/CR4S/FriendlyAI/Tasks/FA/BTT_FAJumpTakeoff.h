#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FAJumpTakeoff.generated.h"

UCLASS()
class CR4S_API UBTT_FAJumpTakeoff : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FAJumpTakeoff();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
