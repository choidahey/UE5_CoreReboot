#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FACollisionToggle.generated.h"

struct FBTService_FACollisionToggleMemory
{
	bool bDoOnceClose = false;
	bool bDoOnceFar = false;
};

UCLASS()
class CR4S_API UBTService_FACollisionToggle : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_FACollisionToggle();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};