#include "BTTask_HelperBotIdle.h"

#include "FriendlyAI/Component/AIJumpComponent.h"

UBTTask_HelperBotIdle::UBTTask_HelperBotIdle()
{
	NodeName = TEXT("HelperBot Idle");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_HelperBotIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AICon = OwnerComp.GetAIOwner())
	{
		if (APawn* Pawn = AICon->GetPawn())
		{
			if (UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>())
			{
				JumpComp->DeactivateJumpComponent();
			}
		}
	}
	
	return EBTNodeResult::InProgress;
}
