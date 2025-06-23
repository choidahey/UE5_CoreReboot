#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_HelperHarvestCrop.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperHarvestCrop : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperHarvestCrop();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ResourceTargetKey;

private:
	UPROPERTY()
	AActor* CachedTarget = nullptr;

	UPROPERTY()
	APawn* CachedHelper = nullptr;
	
	float CachedHarvestDuration = 0.f;
	float ElapsedTime = 0.f;
};