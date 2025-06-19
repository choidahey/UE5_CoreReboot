#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_JumpAndLand.generated.h"

UCLASS()
class CR4S_API UAnimNotifyState_JumpAndLand : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Jump")
	float PredictTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Jump")
	float JumpSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Jump")
	float ArcHeightRatio = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Jump")
	float FallZVelocity = -1000.f;

	UPROPERTY(EditAnywhere, Category = "Jump")
	FName TargetActorKeyName = "TargetActor";

};
