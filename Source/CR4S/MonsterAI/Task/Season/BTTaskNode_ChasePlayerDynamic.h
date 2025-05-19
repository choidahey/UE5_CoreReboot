#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ChasePlayerDynamic.generated.h"

UCLASS()
class CR4S_API UBTTaskNode_ChasePlayerDynamic : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_ChasePlayerDynamic();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Chase")
	float AcceptanceRadius;;

private:
	FAIRequestID MoveRequestID;
	
};
