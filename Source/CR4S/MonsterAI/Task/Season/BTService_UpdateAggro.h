#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateAggro.generated.h"

UCLASS()
class CR4S_API UBTService_UpdateAggro : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateAggro();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp,
						  uint8* NodeMemory,
						  float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector AggroTargetKey;
};
