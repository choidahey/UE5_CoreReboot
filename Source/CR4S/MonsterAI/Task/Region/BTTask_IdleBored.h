#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_IdleBored.generated.h"

UCLASS()
class CR4S_API UBTTask_IdleBored : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_IdleBored();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TArray<TObjectPtr<UAnimMontage>> MontageList;

};
