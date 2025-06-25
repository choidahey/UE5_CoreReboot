#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HelperHarvestCrop.generated.h"

class ACropsGimmick;

USTRUCT()
struct FBTHarvestCropMemory
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget = nullptr;

	UPROPERTY()
	TWeakObjectPtr<APawn> CachedHelper = nullptr;
   
	float CachedHarvestDuration = 0.f;
	float ElapsedTime = 0.f;
	bool bIsTargetDestroyedBound = false;
   
	UPROPERTY()
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerComp = nullptr;
   
	uint8* NodeMemoryPtr = nullptr;

	FBTHarvestCropMemory()
		: CachedTarget(nullptr)
		, CachedHelper(nullptr)
		, CachedHarvestDuration(0.f)
		, ElapsedTime(0.f)
		, bIsTargetDestroyedBound(false)
		, OwnerComp(nullptr)
		, NodeMemoryPtr(nullptr)
	{
	}
};

UCLASS()
class CR4S_API UBTTask_HelperHarvestCrop : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperHarvestCrop();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ResourceTargetKey;

	UPROPERTY(EditAnywhere, Category = "Resource")
	TSubclassOf<ACropsGimmick> TargetCropClass;

protected:
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitMemoryType) const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UFUNCTION() 
	void OnTargetDestroyed(AActor* DestroyedActor);
   
	void CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result);
	void UnbindTargetDestroyedDelegate(uint8* NodeMemory);
};