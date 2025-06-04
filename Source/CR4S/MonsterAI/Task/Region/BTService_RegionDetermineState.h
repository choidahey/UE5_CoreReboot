#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RegionDetermineState.generated.h"

UCLASS()
class CR4S_API UBTService_RegionDetermineState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RegionDetermineState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetActorKey;

};
