#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FARandomFlyChoice.generated.h"

UCLASS()
class CR4S_API UBTT_FARandomFlyChoice : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FARandomFlyChoice();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void PerformRandomChoice(UBehaviorTreeComponent& OwnerComp);
};
