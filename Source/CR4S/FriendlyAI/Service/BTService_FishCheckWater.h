#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FishCheckWater.generated.h"

UCLASS()
class CR4S_API UBTService_FishCheckWater : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FishCheckWater();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector IsInWaterKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector LastValidWaterPositionKey;

private:
	bool IsActorInWaterVolume(AActor* Actor) const;
};