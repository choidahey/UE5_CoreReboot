#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_OutsideSkillRange.generated.h"

UCLASS()
class CR4S_API UBTDecorator_OutsideSkillRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTDecorator_OutsideSkillRange();

protected:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentDistanceKey;
	
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentSkillRangeKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
