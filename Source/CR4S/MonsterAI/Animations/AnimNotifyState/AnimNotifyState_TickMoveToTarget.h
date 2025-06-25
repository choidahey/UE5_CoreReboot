#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_TickMoveToTarget.generated.h"

UCLASS()
class CR4S_API UAnimNotifyState_TickMoveToTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "MoveToTarget")
	bool bUseTargetActor = false;

	UPROPERTY(EditAnywhere, Category = "MoveToTarget", meta = (EditCondition = "bUseTargetActor"))
	FName TargetActorBlackboardKey = "TargetActor";

	UPROPERTY(EditAnywhere, Category = "MoveToTarget", meta = (EditCondition = "!bUseTargetActor"))
	FName BlackboardKeyName = "SkillTargetLocation";

	UPROPERTY(EditAnywhere, Category = "MoveToTarget")
	bool bUseFixedTravelDistance = true;

	UPROPERTY(EditAnywhere, Category = "MoveToTarget", meta = (EditCondition = "bUseFixedTravelDistance"))
	float DesiredTravelDistance = 400.f;

	UPROPERTY(EditAnywhere, Category = "MoveToTarget")
	bool bUseJumpCurve = true;

	UPROPERTY(EditAnywhere, Category = "MoveToTarget", meta = (EditCondition = "bUseJumpCurve"))
	float JumpHeight = 300.f;

	UPROPERTY(EditAnywhere, Category = "MoveToTarget")
	float FrontOffset = 150.f;

private:
	FVector StartLocation;
	FVector TargetLocation;
	float ElapsedTime = 0.f;
	float ActualMoveDuration = 0.f;
	
};
