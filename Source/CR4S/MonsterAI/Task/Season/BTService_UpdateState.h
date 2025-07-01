#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BTService_UpdateState.generated.h"


struct FAIKeys;

UCLASS()
class CR4S_API UBTService_UpdateState : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NearestHouseActorKey;

private:
	float CurrentHPValue = 0.f;
};
