#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BTService_CheckDashDistance.generated.h"

UCLASS()
class CR4S_API UBTService_CheckDashDistance : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckDashDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category="AI|Dash")
	float AwayDistance = 500.f;
	
	UPROPERTY(EditAnywhere, Category="AI|Dash")
	float ForwardDistance = 1100.f;
	
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector bIsDashAwayKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector bIsDashForwardKey;
};
