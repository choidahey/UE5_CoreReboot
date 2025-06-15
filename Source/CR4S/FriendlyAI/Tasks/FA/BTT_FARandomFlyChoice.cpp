#include "BTT_FARandomFlyChoice.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

UBTT_FARandomFlyChoice::UBTT_FARandomFlyChoice()
{
	NodeName = TEXT("FA Random Fly Choice");
	bNotifyTick = false;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_FARandomFlyChoice::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	const float FlyTime = 0.02f;

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindLambda([this, &OwnerComp]()
	{
		PerformRandomChoice(OwnerComp);
	});

	OwnerComp.GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, FlyTime, false);

	return EBTNodeResult::InProgress;
}


void UBTT_FARandomFlyChoice::PerformRandomChoice(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	int32 Rand = UKismetMathLibrary::RandomIntegerInRange(1, 3);
	//int32 Rand = 2;
	if (Rand == 1)
	{
		if (BB->GetValueAsBool("CanThisBirdPerch"))
		{
			BB->SetValueAsBool("IsLookingForPerch", true);
			BB->SetValueAsBool("FlyAway", false);
			BB->SetValueAsBool("TimeToLand", false);
			BB->SetValueAsBool("FlyToPerch", false);
		}
		else
		{
			int32 SubRand = UKismetMathLibrary::RandomIntegerInRange(2, 3);
			if (SubRand == 2)
			{
				BB->SetValueAsBool("FlyAway", true);
				BB->SetValueAsBool("IsLookingForPerch", false);
				BB->SetValueAsBool("TimeToLand", false);
				BB->SetValueAsBool("FlyToPerch", false);
			}
			else
			{
				BB->SetValueAsBool("TimeToLand", true);
				BB->SetValueAsBool("IsLookingForPerch", false);
				BB->SetValueAsBool("FlyAway", false);
				BB->SetValueAsBool("FlyToPerch", false);
			}
		}
	}
	else if (Rand == 2)
	{
		BB->SetValueAsBool("FlyAway", true);
		BB->SetValueAsBool("IsLookingForPerch", false);
		BB->SetValueAsBool("TimeToLand", false);
		BB->SetValueAsBool("FlyToPerch", false);
	}
	else
	{
		BB->SetValueAsBool("TimeToLand", true);
		BB->SetValueAsBool("IsLookingForPerch", false);
		BB->SetValueAsBool("FlyAway", false);
		BB->SetValueAsBool("FlyToPerch", false);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
