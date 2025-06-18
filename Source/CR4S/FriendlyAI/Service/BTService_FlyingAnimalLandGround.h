#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FlyingAnimalLandGround.generated.h"

UCLASS()
class CR4S_API UBTService_FlyingAnimalLandGround : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FlyingAnimalLandGround();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float LandingDistanceThreshold = 2000.0f;
};
