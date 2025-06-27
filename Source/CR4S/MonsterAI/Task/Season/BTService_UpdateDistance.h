#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateDistance.generated.h"

UCLASS()
class CR4S_API UBTService_UpdateDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentDistanceKey;
	
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Skill Selection")
	FBlackboardKeySelector CurrentSkillRangeKey;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugInfo = false;
};