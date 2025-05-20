#include "BTTask_HelperBotBMoveToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "../Component/AIJumpComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"

UBTTask_HelperBotBMoveToTarget::UBTTask_HelperBotBMoveToTarget()
{
	NodeName = TEXT("HelperBot Run EQS And Move To");
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
	if (!AICon || !Pawn || !EQSQueryTemplate)
		return EBTNodeResult::Failed;
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance =
		UEnvQueryManager::RunEQSQuery(
			this,
			EQSQueryTemplate,
			Pawn,
			EEnvQueryRunMode::SingleResult,
			UEnvQueryInstanceBlueprintWrapper::StaticClass()
		);

	if (!QueryInstance)
		return EBTNodeResult::Failed;
	
	if (UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>())
	{
		QueryInstance->GetOnQueryFinishedEvent()
			.AddDynamic(JumpComp, &UAIJumpComponent::OnEQSQueryFinished);
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

// EBTNodeResult::Type UBTTask_HelperBotBMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
// {
// 	AAIController* AICon = OwnerComp.GetAIOwner();
// 	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
// 	if (!AICon || !Pawn || !EQSQueryTemplate)
// 	{
// 		return EBTNodeResult::Failed;
// 	}
//
// 	UEnvQueryInstanceBlueprintWrapper* QueryInstance =
// 		UEnvQueryManager::RunEQSQuery(this, EQSQueryTemplate, Pawn,
// 									  EEnvQueryRunMode::SingleResult, this);
// 	if (!QueryInstance)
// 	{
// 		return EBTNodeResult::Failed;
// 	}
//
// 	if (UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>())
// 	{
// 		QueryInstance->GetOnQueryFinishedEvent()
// 			.AddDynamic(JumpComp, &UAIJumpComponent::OnEQSQueryFinished);
// 		return EBTNodeResult::InProgress;
// 	}
//
// 	return EBTNodeResult::Failed;
// }
