#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FlyingAnimalLandGround.generated.h"

UCLASS()
class CR4S_API UBTTask_FlyingAnimalLandGround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FlyingAnimalLandGround();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere)
	float LandDistanceThreshold = 300.0f;
};
