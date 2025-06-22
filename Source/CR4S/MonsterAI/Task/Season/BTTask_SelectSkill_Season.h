#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SelectSkill_Season.generated.h"

UCLASS()
class CR4S_API UBTTask_SelectSkill_Season : public UBTTask_SelectSkill
{
	GENERATED_BODY()
	
public:
    UBTTask_SelectSkill_Season();

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory) override;

    virtual int32 SelectSkillFromAvailable(
        const TArray<int32>& AvailableSkills,
        AActor* Target) override;

    UPROPERTY(EditAnywhere, Category="Blackboard")
    FBlackboardKeySelector CurrentSkillRangeKey;
    
    UPROPERTY(EditAnywhere, Category="Blackboard")
    FBlackboardKeySelector AttackTypeKey;
};
