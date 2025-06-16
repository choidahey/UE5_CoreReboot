#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FASetToPerchedMode.generated.h"

UCLASS()
class CR4S_API UBTT_FASetToPerchedMode : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FASetToPerchedMode();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
