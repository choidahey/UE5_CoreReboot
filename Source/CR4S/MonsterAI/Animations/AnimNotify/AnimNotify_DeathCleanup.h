#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_DeathCleanup.generated.h"

UCLASS()
class CR4S_API UAnimNotify_DeathCleanup : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	void SpawnDeathEffect(AActor* Actor);

};
