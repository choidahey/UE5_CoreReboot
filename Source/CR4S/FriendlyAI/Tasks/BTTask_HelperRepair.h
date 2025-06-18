#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_HelperRepair.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperRepair : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperRepair();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ResourceTargetKey;

	UFUNCTION()
	void OnTargetDestroyed(AActor* DestroyedActor);

private:
	UPROPERTY()
	AActor* CachedTarget = nullptr;

	UPROPERTY()
	APawn* CachedHelper = nullptr;

	float CachedRepairPerSecond = 0.f;
};