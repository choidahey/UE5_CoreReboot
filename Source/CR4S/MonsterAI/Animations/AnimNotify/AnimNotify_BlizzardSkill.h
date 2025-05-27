#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BlizzardSkill.generated.h"

class ABlizzard;

UCLASS()
class CR4S_API UAnimNotify_BlizzardSkill : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_BlizzardSkill();
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	TSubclassOf<ABlizzard> BlizzardSkillClass;

private:
	FString MyHeader;
};
