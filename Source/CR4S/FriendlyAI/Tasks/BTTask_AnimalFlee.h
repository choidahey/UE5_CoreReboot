#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalFlee.generated.h"

struct FFleeTaskMemory
{
	TArray<AActor*> ThreatsToFlee;
	FTimerHandle ThreatUpdateTimer;
	float LastThreatUpdateTime;
};

UCLASS()
class CR4S_API UBTTask_AnimalFlee : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalFlee();

	virtual uint16 GetInstanceMemorySize() const override;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void UpdateThreats(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY()
	AActor* TargetToFlee;
};