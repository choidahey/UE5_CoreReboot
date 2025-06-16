#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FishPatrolLocation.generated.h"

class UBoxComponent;

UCLASS()
class CR4S_API UBTTask_FishPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FishPatrolLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector PatrolTargetKey;

private:
	FVector FindPatrolLocation(AActor* Fish) const;
	UBoxComponent* FindWaterVolumeBox(AActor* Fish) const;
	FVector GetRandomLocationInBox(UBoxComponent* BoxComp, const FVector& CurrentLocation, const FVector& ForwardVector) const;
};