#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AMChase.generated.h"

UCLASS()
class CR4S_API UBTTask_AMChase : public UBTTaskNode
{
	GENERATED_BODY()

// public:
// 	UBTTask_AMChase();
//
// 	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
// 	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
//
// 	EBTNodeResult::Type CurrentResult = EBTNodeResult::InProgress;
//
// protected:
// 	UPROPERTY(EditAnywhere)
// 	float AcceptanceRadius = 0.1f;
//
// 	UPROPERTY()
// 	AActor* TargetToChase;
};
