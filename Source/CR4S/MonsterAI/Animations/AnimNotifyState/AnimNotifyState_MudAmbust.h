#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_MudAmbust.generated.h"

UCLASS()
class CR4S_API UAnimNotifyState_MudAmbust : public UAnimNotifyState
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
	
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	TSubclassOf<AActor> MudFieldClass;

	UPROPERTY(EditAnywhere, Category="MudAmbust")
	float TraceStartZ = 500.f;
	UPROPERTY(EditAnywhere, Category="MudAmbust")
	float TraceDownDistance = 1000.f;

private:
	TWeakObjectPtr<AActor> TargetActor;
	TWeakObjectPtr<AActor> SpawnedMudField;
	FTimerHandle RiseTimerHandle;

	bool bIsTriggeredReverse = false;
	
	FVector StartLocation;
	FVector TargetLocation;
	
	float Duration;
	float ElapsedTime;
};
