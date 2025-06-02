#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Task/BTTask_SelectSkill.h"
#include "BTTask_SelectSkillLavaBrute.generated.h"

UCLASS()
class CR4S_API UBTTask_SelectSkillLavaBrute : public UBTTask_SelectSkill
{
	GENERATED_BODY()
	
protected:
	virtual int32 SelectSkillFromAvailable(const TArray<int32>& AvailableSkills, AActor* Target) override;

};
