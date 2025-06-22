#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FAPerchCheck.generated.h"

struct FBTService_FAPerchCheckMemory
{
	bool bDidSetFlyToPerch = false;
};

UCLASS()
class CR4S_API UBTService_FAPerchCheck : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_FAPerchCheck();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};