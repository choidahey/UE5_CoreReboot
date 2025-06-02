#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AnimalIdle.generated.h"

UCLASS()
class CR4S_API UBTTask_AnimalIdle : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AnimalIdle();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere)
	float MinIdleTime = 1.0f;

	UPROPERTY(EditAnywhere)
	float MaxIdleTime = 3.0f;

private:
	struct FIdleMemory
	{
		float WaitTime = 0.f;
		float Elapsed = 0.f;
	};
};
