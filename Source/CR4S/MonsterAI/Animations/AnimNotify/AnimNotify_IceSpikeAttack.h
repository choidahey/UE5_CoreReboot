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
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	TSubclassOf<AActor> SpawnActorClass;

private:
	FString MyHeader = TEXT("Notify_IceSpike");
};
