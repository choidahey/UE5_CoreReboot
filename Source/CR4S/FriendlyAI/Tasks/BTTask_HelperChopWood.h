#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_HelperChopWood.generated.h"

USTRUCT()
struct FBTChopWoodMemory
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget = nullptr;

	UPROPERTY()
	TWeakObjectPtr<APawn> CachedHelper = nullptr;
	
	float CachedDamagePerSecond = 0.f;

	FBTChopWoodMemory()
		: CachedTarget(nullptr)
		, CachedHelper(nullptr)
		, CachedDamagePerSecond(0.f)
	{
	}
};

UCLASS()
class CR4S_API UBTTask_HelperChopWood : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperChopWood();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ResourceTargetKey;

private:
	void CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result);
};