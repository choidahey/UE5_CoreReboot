#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RandomDice.generated.h"

UCLASS()
class CR4S_API UBTService_RandomDice : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RandomDice();

	UPROPERTY(EditAnywhere, Category = "RandomIndex")
	int32 NumDice = 3;

	UPROPERTY(EditAnywhere, Category="RandomIndex")
	FBlackboardKeySelector RandomIndexKey;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
