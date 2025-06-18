#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FAWaitPerchDuration.generated.h"

UCLASS()
class CR4S_API UBTT_FAWaitPerchDuration : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FAWaitPerchDuration();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void FinishDelay(UBehaviorTreeComponent* OwnerComp);

	UPROPERTY()
	FTimerHandle DelayHandle;
};
