#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "BTTask_HelperBotBMoveToTarget.generated.h"

class UEnvQuery;

UCLASS()
class CR4S_API UBTTask_HelperBotBMoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotBMoveToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category="EQS")
	UEnvQuery* EQSQueryTemplate;
	
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector ResultLocationKey;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float AcceptanceRadius = 50.f;
};
