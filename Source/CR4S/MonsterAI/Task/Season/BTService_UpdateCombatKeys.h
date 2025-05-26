#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateCombatKeys.generated.h"


UCLASS()
class CR4S_API UBTService_UpdateCombatKeys : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateCombatKeys();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HouseActor;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bIsPlayerAttackRange;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bIsHouseAttackRange;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CurrentDistance;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (ClampMin = "0.0"))
	float AttackDistanceThreshold = 800.f;
};
