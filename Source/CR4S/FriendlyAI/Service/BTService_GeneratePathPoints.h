#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GeneratePathPoints.generated.h"

UCLASS()
class CR4S_API UBTService_GeneratePathPoints : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_GeneratePathPoints();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Path")
	int32 NumPoints = 5;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetPointKey;
};
