#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAttackMontage.generated.h"

class UMonsterSkillComponent;

UCLASS()
class CR4S_API UBTTask_PlayAttackMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayAttackMontage();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UFUNCTION()
	void OnMontageEnded(bool bInterrupted);

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillIndexKey;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY()
	TObjectPtr<UMonsterSkillComponent> CachedSkillComp;

};
