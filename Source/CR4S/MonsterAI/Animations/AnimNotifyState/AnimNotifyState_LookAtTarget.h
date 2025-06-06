#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_LookAtTarget.generated.h"

UCLASS()
class CR4S_API UAnimNotifyState_LookAtTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "LookAt")
	bool bRotateBodyYaw = true;

	UPROPERTY(EditAnywhere, Category = "LookAt")
	bool bRotateHeadPitch = false;

	UPROPERTY(EditAnywhere, Category = "LookAt")
	bool bResetRotationOnEnd = false;

	UPROPERTY(EditAnywhere, Category = "LookAt")
	float InterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "LookAt", meta = (EditCondition = "bRotateHeadPitch"))
	float MaxPitchAngle = 20.f;

	UPROPERTY(EditAnywhere, Category = "LookAt")
	FName TargetBlackboardKey = TEXT("TargetActor");

protected:
	TWeakObjectPtr<AActor> CachedOwner;
	TWeakObjectPtr<AController> CachedController;

	FRotator InitialActorRotation;
	FRotator InitialControlRotation;

};
