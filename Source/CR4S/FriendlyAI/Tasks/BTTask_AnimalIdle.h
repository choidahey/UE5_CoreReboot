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
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY()
	class UAnimalAnimInstance* AnimalAnimInst;
	
	UBehaviorTreeComponent* CachedOwnerComp;

	UPROPERTY()
	UAnimMontage* CurrentMontage;

	UFUNCTION()
	void OnIdleMontageFinished(UAnimMontage* Montage, bool bInterrupted);
};
