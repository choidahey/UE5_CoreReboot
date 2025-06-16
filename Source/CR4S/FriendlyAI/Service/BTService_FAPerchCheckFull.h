#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FAPerchCheckFull.generated.h"

UCLASS()
class CR4S_API UBTService_FAPerchCheckFull : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FAPerchCheckFull();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
