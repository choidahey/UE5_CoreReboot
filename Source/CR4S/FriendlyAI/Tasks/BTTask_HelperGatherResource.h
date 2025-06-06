#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HelperGatherResource.generated.h"

class ADestructibleGimmick;  

UCLASS()
class CR4S_API UBTTask_HelperGatherResource : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperGatherResource();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector	ResourceTargetKey;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<ADestructibleGimmick> TargetResourceClass;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask   (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UFUNCTION() void OnTargetDestroyed(AActor* DestroyedActor);
	
	UPROPERTY()	AActor*	CachedTarget = nullptr;
	UPROPERTY()	APawn*	CachedHelper = nullptr;
	float CachedDamagePerSecond	= 0.f;
};
