#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToBaseCampLocation.generated.h"

UCLASS()
class CR4S_API UBTTask_MoveToBaseCampLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToBaseCampLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> ReturnMontage;	
};
