#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AITypes.h"
#include "Engine/TimerHandle.h"
#include "BTTask_HelperBotBMoveToResource.generated.h"

USTRUCT()
struct FBTMoveToResourceMemory
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UEnvQueryInstanceBlueprintWrapper> QueryInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> OwnerCompPtr = nullptr;

	UPROPERTY()
	TObjectPtr<class UAIJumpComponent> JumpComponent = nullptr;

	FTimerHandle CheckTimer;

	FBTMoveToResourceMemory()
		: QueryInstance(nullptr)
		, OwnerCompPtr(nullptr)
		, JumpComponent(nullptr)
	{
	}
};

UCLASS()
class CR4S_API UBTTask_HelperBotBMoveToResource : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotBMoveToResource();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere)
	UEnvQuery* ResourceQuery;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ResourceLocationKey;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ResourceTargetKey;
	
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 30.f;

private:
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* Wrapper, EEnvQueryStatus::Type Status);

	void CheckReachedTarget(FBTMoveToResourceMemory* MyMemory);
};