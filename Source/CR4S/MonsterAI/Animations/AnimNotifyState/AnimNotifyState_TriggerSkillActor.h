#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_TriggerSkillActor.generated.h"

UCLASS()
class CR4S_API UAnimNotifyState_TriggerSkillActor : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	FName SkillTriggerTag = TEXT("SkillActor");

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	FName ActivateFunctionName = TEXT("ActivateSkill");

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	FName EndFunctionName = TEXT("EndSkill");

};
