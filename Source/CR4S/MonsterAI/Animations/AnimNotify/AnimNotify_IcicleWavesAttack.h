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

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	int32 SpawnCount = 3;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float BaseDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float DistanceStep = 400.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float SpawnInterval = 1.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	bool bForward = true;

private:
	FString MyHeader = TEXT("Nofity_IcicleWaves");
};
