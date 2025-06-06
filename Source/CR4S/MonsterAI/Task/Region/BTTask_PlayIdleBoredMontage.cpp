#include "MonsterAI/Task/Region/BTTask_PlayIdleBoredMontage.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTTask_PlayIdleBoredMontage::UBTTask_PlayIdleBoredMontage()
{
	NodeName = TEXT("PlayIdleBoredMontage");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PlayIdleBoredMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FMontageMemory* Memory = (FMontageMemory*)NodeMemory;

	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Monster || MontageList.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	UMonsterAnimComponent* AnimComp = Monster->FindComponentByClass<UMonsterAnimComponent>();
	if (!AnimComp || AnimComp->IsAnyMontagePlaying())
	{
		return EBTNodeResult::Failed;
	}

	const int32 Index = FMath::RandRange(0, MontageList.Num() - 1);
	UAnimMontage* SelectedMontage = MontageList[Index];
	AnimComp->PlayMontage(SelectedMontage);

	if (!bWaitUntilFinished)
	{
		return EBTNodeResult::Succeeded;
	}

	Memory->AnimComp = AnimComp;
	Memory->PlayingMontage = SelectedMontage;
	Memory->bPlaying = true;

	return EBTNodeResult::InProgress;
}

void UBTTask_PlayIdleBoredMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FMontageMemory* Memory = (FMontageMemory*)NodeMemory;

	if (!Memory->bPlaying || !Memory->AnimComp.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!Memory->AnimComp->IsPlayingMontage(Memory->PlayingMontage))
	{
		if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
		{
			BB->SetValueAsFloat(TEXT("LastIdleBoredTime"), OwnerComp.GetOwner()->GetWorld()->GetTimeSeconds());
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
