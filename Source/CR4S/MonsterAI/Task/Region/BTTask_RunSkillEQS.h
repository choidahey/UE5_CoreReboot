#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "MonsterAI/Data/MonsterEnum.h"
#include "BTTask_RunSkillEQS.generated.h"

UCLASS()
class CR4S_API UBTTask_RunSkillEQS : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RunSkillEQS();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void RunEQS(UEnvQuery* EQS, UObject* QueryOwner);
	void OnEQSFinished(TSharedPtr<FEnvQueryResult> Result);
	EApproachType DecideApproachType(const TArray<EApproachType>& Candidates, const FVector& From, const FVector& To) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillIndexKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillTargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ApproachTypeKey;

private:
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	int32 CurrentSkillIndex = INDEX_NONE;
	EApproachType CurrentApproachType = EApproachType::Stand;
	FVector SkillTargetLocation = FVector::ZeroVector;
};
