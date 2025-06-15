#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FASetToFlyingMode.generated.h"

UCLASS()
class CR4S_API UBTT_FASetToFlyingMode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FASetToFlyingMode();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
