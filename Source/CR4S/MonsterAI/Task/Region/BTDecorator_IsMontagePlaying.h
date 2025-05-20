#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsMontagePlaying.generated.h"

UCLASS()
class CR4S_API UBTDecorator_IsMontagePlaying : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsMontagePlaying();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bShouldBePlaying = false;

};
