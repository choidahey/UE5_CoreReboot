#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../HelperBotState.h"
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

protected:
	void RunBotLogic();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBlackboardData* BlackboardAsset;
	
	UPROPERTY(EditDefaultsOnly)
	FName StateKeyName = TEXT("HelperBotState");
};