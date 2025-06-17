#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FAChargeAttack.generated.h"

UCLASS()
class CR4S_API UBTT_FAChargeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FAChargeAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	UPROPERTY(EditAnywhere, Category = "Charge Attack")
	float ChargeSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Charge Attack")
	float ArrivalThreshold = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Charge Attack")
	float LineTraceDistance = 5000.0f;
};

struct FBTTChargeAttackMemory
{
	FVector TargetLocation;
	bool bIsCharging;
	
	FBTTChargeAttackMemory()
	{
		TargetLocation = FVector::ZeroVector;
		bIsCharging = false;
	}
};