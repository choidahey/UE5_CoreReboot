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
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY()
	float PatrolRadius = 2000.f;

	UPROPERTY()
	float AcceptanceRadius = 200.f;

	UPROPERTY()
	float MaxWaitTime = 10.f;

	UPROPERTY()
	float DecelerationDistance = 300.f;

	UPROPERTY()
	float SpeedInterpSpeed = 3.f;

private:
	struct FPatrolMemory
	{
		FVector PatrolDestination;
		float TotalDistance = 0.f;
		float CurrentSpeed = 0.f;
		float ElapsedTime = 0.f;
		uint8 Pattern = 0;
		bool bSwitched = false;
	};
};