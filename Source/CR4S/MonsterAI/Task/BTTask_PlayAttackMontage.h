#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAttackMontage.generated.h"

UCLASS()
class CR4S_API UBTTask_PlayAttackMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayAttackMontage();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnAnimMontageEnded(UAnimMontage* Montage);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SkillIndexKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bIsPlayingAttackMontage;

private:
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp = nullptr;

	FString MyHeader = TEXT("UBTTask_PlayAttackMontage");
};
