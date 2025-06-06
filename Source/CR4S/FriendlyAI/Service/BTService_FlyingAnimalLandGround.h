#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FlyingAnimalLandGround.generated.h"

UCLASS()
class CR4S_API UBTService_FlyingAnimalLandGround : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FlyingAnimalLandGround();

protected:
	UPROPERTY(EditAnywhere)
	float LandDistanceThreshold = 300.0f;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
