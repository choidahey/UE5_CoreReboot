#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayIdleBoredMontage.generated.h"

UCLASS()
class CR4S_API UBTTask_PlayIdleBoredMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayIdleBoredMontage();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FMontageMemory); }

	UPROPERTY(EditAnywhere, Category = "Montage")
	TArray<TObjectPtr<UAnimMontage>> MontageList;

	UPROPERTY(EditAnywhere, Category = "Montage")
	bool bWaitUntilFinished = true;

private:
	struct FMontageMemory
	{
		bool bPlaying = false;
		TWeakObjectPtr<class UMonsterAnimComponent> AnimComp;
		UAnimMontage* PlayingMontage = nullptr;
	};
};
