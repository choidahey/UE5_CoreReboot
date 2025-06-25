#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Engine/TimerHandle.h"
#include "BTTask_HelperBotBMoveToTarget.generated.h"

USTRUCT()
struct FBTMoveToTargetMemory
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> OwnerCompPtr = nullptr;

	UPROPERTY()
	TObjectPtr<class UAIJumpComponent> JumpComponent = nullptr;

	FTimerHandle CheckTimer;
	FTimerHandle RotationTimer;

	FBTMoveToTargetMemory()
		: OwnerCompPtr(nullptr)
		, JumpComponent(nullptr)
	{
	}
};

UCLASS()
class CR4S_API UBTTask_HelperBotBMoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotBMoveToTarget();
   
	UPROPERTY(EditAnywhere, Category="Follow")
	float DistanceThreshold = 50.f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	void CheckDistanceAndRotation(FBTMoveToTargetMemory* MyMemory);
	void HandleTargetRotation(FBTMoveToTargetMemory* MyMemory);
};