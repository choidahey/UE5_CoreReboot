#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FAFlyGroundTarget.generated.h"

UCLASS()
class CR4S_API UBTService_FAFlyGroundTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FAFlyGroundTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float CurrentYawSpeed = 0.f;
	float CurrentPitchSpeed = 0.f;
	float CurrentRollSpeed = 0.f;
};
