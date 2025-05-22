#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TestBTT_GFMove.generated.h"

UCLASS()
class CR4S_API UTestBTT_GFMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTestBTT_GFMove();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
											uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
						  uint8* NodeMemory,
						  float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	struct FBlackboardKeySelector TargetLocationKey;

private:
	bool bIsFlying;
	
	bool IsFlightFinished(class ACharacter* Character) const;
	
	bool IsGroundMoveFinished(class AAIController* Controller) const;
};
