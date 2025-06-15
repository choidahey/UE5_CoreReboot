#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FAFlyToPerchTarget.generated.h"

USTRUCT()
struct FBTService_FAFlyToPerchTargetMemory
{
	GENERATED_BODY()

	float CurrentYawSpeed = 0.0f;
	float CurrentRollSpeed = 0.0f;
	float CurrentPitchSpeed = 0.0f;
};

UCLASS()
class CR4S_API UBTService_FAFlyToPerchTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FAFlyToPerchTarget();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};