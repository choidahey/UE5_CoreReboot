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
	
public:
	UPROPERTY(EditAnywhere)
	float IdleTimeMin = 1.0f;
	
	UPROPERTY(EditAnywhere)
	float IdleTimeMax = 3.0f;
	
	UPROPERTY(EditAnywhere)
	float PatrolRadius = 2000.0f;

private:
	FVector PatrolDestination;
	
	float WaitTime = 0.0f;
	
	float WaitElapsed = 0.0f;
	
	uint8 bMoving = false;
};