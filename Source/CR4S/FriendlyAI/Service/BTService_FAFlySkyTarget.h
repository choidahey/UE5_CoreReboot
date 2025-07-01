#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FAFlySkyTarget.generated.h"

struct FBTService_FAFlySkyTargetMemory
{
	float CurrentYawSpeed = 0.f;
	float CurrentPitchSpeed = 0.f;
	float CurrentRollSpeed = 0.f;
};

UCLASS()
class CR4S_API UBTService_FAFlySkyTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FAFlySkyTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};