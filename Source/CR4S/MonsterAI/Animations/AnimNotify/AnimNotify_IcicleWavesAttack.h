#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_IcicleWavesAttack.generated.h"


UCLASS()
class CR4S_API UAnimNotify_IcicleWavesAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_IcicleWavesAttack();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	TSubclassOf<AActor> IcicleWaveActorClass;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	int32 SpawnCount;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float BaseDistance;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float DistanceStep;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float SpawnInterval;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	bool bForward = true;
};
