#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../HelperBotState.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "HelperBotAIController.generated.h"

class UBehaviorTree;
class UBlackboardData;

UCLASS()
class CR4S_API AHelperBotAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHelperBotAIController();

	virtual void BeginPlay() override;
	
	void SetBotState(EHelperBotState NewState);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpHeight = 300.f;

protected:
	void RunBotLogic();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBlackboardData* BlackboardAsset;
	
	UPROPERTY(EditDefaultsOnly)
	FName StateKeyName = TEXT("HelperBotState");

	UPROPERTY(EditDefaultsOnly)
	FName AttackTargetKeyName = TEXT("AttackTarget");

	
public:
	void RunEQSQuery();

protected:
	void OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
};