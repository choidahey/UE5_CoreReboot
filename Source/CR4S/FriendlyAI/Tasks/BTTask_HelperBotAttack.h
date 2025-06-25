#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_HelperBotAttack.generated.h"

USTRUCT()
struct FBTAttackMemory
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> CachedTarget = nullptr;

	UPROPERTY()
	TWeakObjectPtr<APawn> CachedHelper = nullptr;
	
	float CachedDamagePerSecond = 0.f;
	float AttackTimer = 0.f;

	FBTAttackMemory()
		: CachedTarget(nullptr)
		, CachedHelper(nullptr)
		, CachedDamagePerSecond(0.f)
		, AttackTimer(0.f)
	{
	}
};

UCLASS()
class CR4S_API UBTTask_HelperBotAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperBotAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange = 500.0f;

private:
	const float MaxAttackDuration = 5.0f;
	
	AActor* FindNearestEnemy(APawn* Helper);
	void CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result);
};