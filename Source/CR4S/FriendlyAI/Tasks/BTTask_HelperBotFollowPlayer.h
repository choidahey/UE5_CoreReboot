#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BTTask_HelperBotFollowPlayer.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperBotFollowPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotFollowPlayer();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};