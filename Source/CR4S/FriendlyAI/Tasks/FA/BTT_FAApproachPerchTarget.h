#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FAApproachPerchTarget.generated.h"

struct FBTT_FAApproachPerchTargetMemory
{
	FVector StartLocation;
	FRotator StartRotation;
	FVector TargetLocation;
	FRotator TargetRotation;
	float CurrentAlpha = 0.0f;
};

UCLASS()
class CR4S_API UBTT_FAApproachPerchTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FAApproachPerchTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};
