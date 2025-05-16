#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"
#include "BTTask_HelperBotFollowPlayerEQS.generated.h"

class UEnvQueryInstanceBlueprintWrapper;

UCLASS()
class CR4S_API UBTTask_HelperBotFollowPlayerEQS : public UBTTask_RunEQSQuery
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UFUNCTION()
	virtual void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
public:
	UBTTask_HelperBotFollowPlayerEQS(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptableRadius = 100.f;

private:
	UBehaviorTreeComponent* CachedOwnerComp = nullptr;
	AAIController* CachedController = nullptr;
};
