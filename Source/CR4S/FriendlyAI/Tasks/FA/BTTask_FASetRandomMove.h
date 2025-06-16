#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FASetRandomMove.generated.h"

UCLASS()
class CR4S_API UBTTask_FASetRandomMove : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FASetRandomMove();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override {}

protected:
	FRotator TargetRotation;
	float RotationInterpSpeed = 2.f;
};
