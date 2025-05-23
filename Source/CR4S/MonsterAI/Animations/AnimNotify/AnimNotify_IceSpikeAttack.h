#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_IceSpikeAttack.generated.h"

class AIceSpike;

UCLASS()
class CR4S_API UAnimNotify_IceSpikeAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_IceSpikeAttack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Actor Class")
	TSubclassOf<AIceSpike> IceSpikeActor;

	//UPROPERTY(EditAnywhere, Category = "NotifyID")
	//FName NotifyID

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	FString MyHeader;
};
