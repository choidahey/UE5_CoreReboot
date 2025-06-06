#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_MoveToCharacter.generated.h"

class AAIController;

UCLASS()
class CR4S_API UAnimNotifyState_MoveToCharacter : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	void StartMoveTo();
	void LaunchJumpTowardTarget();
	AActor* GetTargetActor() const;

public:
	UPROPERTY(EditAnywhere, Category = "MoveTo")
	float MoveSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "MoveTo")
	float MoveInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category = "MoveTo")
	float AcceptableRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "MoveTo")
	bool bUseTargetActor = true;

	UPROPERTY(EditAnywhere, Category = "MoveTo|Jump")
	bool bJumpOnEnd = false;

	UPROPERTY(EditAnywhere, Category = "MoveTo|Jump", meta = (EditCondition = "bJumpOnEnd"))
	float MaxJumpDistance = 1200.f;

	UPROPERTY(EditAnywhere, Category = "MoveTo|Jump", meta = (EditCondition = "bJumpOnEnd"))
	float JumpZImpulse = 600.f;

	UPROPERTY(EditAnywhere, Category = "MoveTo")
	FVector MoveToLocation;

	UPROPERTY(EditAnywhere, Category = "MoveTo")
	FName TargetBlackboardKey = "TargetActor";

protected:
	TWeakObjectPtr<ACharacter> CachedCharacter;
	TWeakObjectPtr<AAIController> CachedController;
	AActor* TargetActor = nullptr;
	float ElapsedMoveTime = 0.f;

};
