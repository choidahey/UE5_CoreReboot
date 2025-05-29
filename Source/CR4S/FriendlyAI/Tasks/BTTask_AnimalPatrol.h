#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalPatrol.generated.h"

UCLASS()
class CR4S_API UBTTask_AnimalPatrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalPatrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere)
	float PatrolRadius = 2000.0f;

	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 100.0f;

	UPROPERTY(EditAnywhere)
	float MaxWaitTime = 5.0f;

private:
	struct FPatrolMemory
	{
		FVector PatrolDestination;
		float Elapsed = 0.f;
	};
};
