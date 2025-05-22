#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HelperBotBMoveToTarget.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperBotBMoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotBMoveToTarget();
	
	UPROPERTY(EditAnywhere, Category="Follow")
	float DistanceThreshold = 100.f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};