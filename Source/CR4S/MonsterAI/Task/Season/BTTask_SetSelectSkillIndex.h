#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SetSelectSkillIndex.generated.h"

UCLASS()
class CR4S_API UBTTask_SetSelectSkillIndex : public UBTTask_SelectSkill
{
	GENERATED_BODY()
	
public:
    UBTTask_SetSelectSkillIndex();

protected:
    virtual int32 SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ClampMin = "0"))
	int32 SetSkillIndex = 0;
};
