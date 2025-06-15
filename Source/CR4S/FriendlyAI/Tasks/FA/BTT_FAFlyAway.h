#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FAFlyAway.generated.h"

UCLASS()
class CR4S_API UBTT_FAFlyAway : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FAFlyAway();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool bFlyAway = true;
};
