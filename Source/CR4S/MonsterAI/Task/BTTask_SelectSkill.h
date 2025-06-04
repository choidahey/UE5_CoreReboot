#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectSkill.generated.h"

UCLASS()
class CR4S_API UBTTask_SelectSkill : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual int32 SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target);

	UPROPERTY()
	TWeakObjectPtr<class ABaseMonster> CachedMonster;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillIndex;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bIsPlayingAttackMontage;


};
