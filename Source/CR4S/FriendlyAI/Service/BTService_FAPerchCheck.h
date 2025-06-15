#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FAPerchCheck.generated.h"

UCLASS()
class CR4S_API UBTService_FAPerchCheck : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FAPerchCheck();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bDidSetFlyToPerch = false;
};
