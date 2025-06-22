#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayAnyMontage.generated.h"

struct FBTTask_PlayMontageMemory
{
    TObjectPtr<UAnimMontage> PlayingMontage;
    
    float MontageStartTime;
    float MontageDuration;
    bool bMontageFinished;
    float bPreviousMaxSpeed;
    bool bPreviousAllowRotation;

    FBTTask_PlayMontageMemory()
    {
        Reset();
    }

    void Reset()
    {
        PlayingMontage = nullptr;
        MontageStartTime = 0.0f;
        MontageDuration = 0.0f;
        bMontageFinished = false;
        bPreviousMaxSpeed = 600.0f;
        bPreviousAllowRotation = true;
    }
};


UCLASS()
class CR4S_API UBTTask_PlayAnyMontage : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_PlayAnyMontage();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
    virtual uint16 GetInstanceMemorySize() const override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> MontageToPlay;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float PlayRate = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    FName StartSectionName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bWaitForFinish = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bAllowMovement = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bAllowRotation = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bStopMontageOnAbort = true;

private:
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
