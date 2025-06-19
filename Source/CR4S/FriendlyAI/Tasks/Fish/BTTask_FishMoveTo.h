#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FishMoveTo.generated.h"

UCLASS()
class CR4S_API UBTTask_FishMoveTo : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FishMoveTo();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector PatrolTargetKey;

private:
	struct FFishMoveToMemory
	{
		FVector TargetLocation;
		float StartTime;
		float NextWiggleTime;
		FVector WiggleOffset;
	};

	bool HasReachedTarget(const FVector& CurrentLocation, const FVector& TargetLocation) const;
	FVector GetWiggleMovement(float CurrentTime, FFishMoveToMemory* TaskMemory) const;
};