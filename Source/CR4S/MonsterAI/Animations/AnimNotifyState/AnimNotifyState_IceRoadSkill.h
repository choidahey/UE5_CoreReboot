#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_IceRoadSkill.generated.h"


UCLASS()
class CR4S_API UAnimNotifyState_IceRoadSkill : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration) override;

    virtual void NotifyTick(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float FrameDeltaTime) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation) override;

    UPROPERTY(EditAnywhere, Category = "IceRoad")
    float ForwardDistance = 500.f;

    UPROPERTY(EditAnywhere, Category = "IceRoad")
    float AwayDistance = 800.f;

private:
    FVector StartLocation;
    FVector TargetLocation;
    float   ElapsedTime = 0.f;
    float   Duration = 0.f;
};
