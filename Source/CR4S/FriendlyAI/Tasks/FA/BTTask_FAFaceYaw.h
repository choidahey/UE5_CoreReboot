#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FAFaceYaw.generated.h"

UCLASS()
class CR4S_API UBTTask_FAFaceYaw : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FAFaceYaw();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
