#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAttackMontage.generated.h"

UCLASS()
class CR4S_API UBTTask_PlayAttackMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayAttackMontage();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillIndexKey;

};
