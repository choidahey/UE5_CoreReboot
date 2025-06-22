#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnActorDelay.generated.h"

class AIceSpike;

UCLASS()
class CR4S_API UAnimNotify_SpawnActorDelay : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float SpawnDelayTime = 0.5f;

private:
	UPROPERTY()
	AActor* TargetActor = nullptr;
	FTimerHandle SpawnTimerHandle;
};
