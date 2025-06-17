#include "BTT_FAWaitPerchDuration.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"

UBTT_FAWaitPerchDuration::UBTT_FAWaitPerchDuration()
{
	NodeName = TEXT("Fly: Wait Perch Duration");
	bNotifyTick = false;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_FAWaitPerchDuration::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	const float Duration = BB->GetValueAsFloat("PerchTime");

	if (Duration <= 0.f)
	{
		return EBTNodeResult::Succeeded;
	}

	FTimerDelegate DelayDelegate;
	DelayDelegate.BindUObject(this, &UBTT_FAWaitPerchDuration::FinishDelay, &OwnerComp);

	OwnerComp.GetWorld()->GetTimerManager().SetTimer(DelayHandle, DelayDelegate, Duration, false);

	return EBTNodeResult::InProgress;
}

void UBTT_FAWaitPerchDuration::FinishDelay(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
