#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FAUpdateSkyTarget.generated.h"

UCLASS()
class CR4S_API UBTService_FAUpdateSkyTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FAUpdateSkyTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BirdSkyTargetsKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector NextSkyTargetKey;

	UPROPERTY(EditAnywhere)
	float DistanceThreshold = 3000.0f;
};
