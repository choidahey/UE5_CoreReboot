#include "BTTask_HelperBotIdle.h"

UBTTask_HelperBotIdle::UBTTask_HelperBotIdle()
{
	NodeName = TEXT("HelperBot Idle");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_HelperBotIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}
