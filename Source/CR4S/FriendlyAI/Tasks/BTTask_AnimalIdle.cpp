#include "BTTask_AnimalIdle.h"

UBTTask_AnimalIdle::UBTTask_AnimalIdle()
{
	NodeName = TEXT("Animal Idle");
	bNotifyTick = true;
}

uint16 UBTTask_AnimalIdle::GetInstanceMemorySize() const
{
	return sizeof(FIdleMemory);
}

EBTNodeResult::Type UBTTask_AnimalIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FIdleMemory* Mem = (FIdleMemory*)NodeMemory;
	Mem->WaitTime = FMath::FRandRange(MinIdleTime, MaxIdleTime);
	Mem->Elapsed = 0.f;
	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalIdle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FIdleMemory* Mem = (FIdleMemory*)NodeMemory;
	Mem->Elapsed += DeltaSeconds;

	if (Mem->Elapsed >= Mem->WaitTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
