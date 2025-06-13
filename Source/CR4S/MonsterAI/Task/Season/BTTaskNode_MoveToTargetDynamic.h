#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterAI/Data/MonsterEnum.h"
#include "BTTaskNode_MoveToTargetDynamic.generated.h"


UCLASS()
class CR4S_API UBTTaskNode_MoveToTargetDynamic : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_MoveToTargetDynamic();
	float GetCurrentAcceptanceRadius(FName KeyName, UBlackboardComponent* BB) const;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerAcceptanceRadiusKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HouseAcceptanceRadiusKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float DefaultAcceptanceRadius = 1000.f;
};
