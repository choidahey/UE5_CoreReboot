#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_IcicleWavesAttack.generated.h"


UCLASS()
class CR4S_API UAnimNotify_IcicleWavesAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	TSubclassOf<AActor> IcicleWaveActorClass;

	UPROPERTY(EditInstanceOnly, Category = "Boss|Skill", meta=(ClampMin="1"))
	int32 SpawnCount = 3;

	UPROPERTY(EditInstanceOnly, Category = "Boss|Skill")
	float BaseDistance = 500.f;

	UPROPERTY(EditInstanceOnly, Category = "Boss|Skill")
	float DistanceStep = 400.f;

	UPROPERTY(EditInstanceOnly, Category = "Boss|Skill")
	float SpawnInterval = 1.f;

	UPROPERTY(EditInstanceOnly, Category = "Boss|Skill")
	bool bForward = true;
};
