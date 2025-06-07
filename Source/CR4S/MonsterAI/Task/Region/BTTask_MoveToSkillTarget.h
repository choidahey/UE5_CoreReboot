#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MonsterAI/Data/MonsterEnum.h"
#include "BTTask_MoveToSkillTarget.generated.h"

UCLASS()
class CR4S_API UBTTask_MoveToSkillTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToSkillTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillTargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ApproachTypeKey;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptableRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SlowMoveSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashMoveSpeed = 1800.f;

private:
	FVector TargetLocation = FVector::ZeroVector;
	EApproachType ApproachType = EApproachType::Stand;

};
