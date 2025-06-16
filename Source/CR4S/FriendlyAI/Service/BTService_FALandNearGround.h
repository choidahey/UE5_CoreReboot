#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FALandNearGround.generated.h"

UCLASS()
class CR4S_API UBTService_FALandNearGround : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FALandNearGround();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTService_FALandNearGroundMemory); }

private:
	struct FBTService_FALandNearGroundMemory
	{
		bool bHasLanded = false;
	};
};