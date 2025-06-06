#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateState.generated.h"


UCLASS()
class CR4S_API UBTService_UpdateState : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HouseActor;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float AttackDistanceThreshold = 1000.f;


private:
	bool bIsPlayerInAttackRange = false;
	bool bIsHouseInAttackRange = false;
};
