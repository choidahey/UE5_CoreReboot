#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_PlayAttackMontage.generated.h"

class UAnimMontage;
class UMonsterSkillComponent;

UCLASS()
class CR4S_API UBTTaskNode_PlayAttackMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_PlayAttackMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
