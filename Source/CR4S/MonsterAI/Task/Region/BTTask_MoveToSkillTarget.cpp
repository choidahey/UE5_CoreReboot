#include "MonsterAI/Task/Region/BTTask_MoveToSkillTarget.h"
#include "AIController.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/Data/MonsterEnum.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_MoveToSkillTarget::UBTTask_MoveToSkillTarget()
{
	NodeName = TEXT("Move To Skill Target");

	bNotifyTick = true;

}

EBTNodeResult::Type UBTTask_MoveToSkillTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!IsValid(Monster)) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	UCharacterMovementComponent* MoveComp = Monster->FindComponentByClass<UCharacterMovementComponent>();
	if (!MoveComp) return EBTNodeResult::Failed;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(AcceptableRadius);

	const EPathFollowingRequestResult::Type Result = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
	return (Result == EPathFollowingRequestResult::Failed) ? EBTNodeResult::Failed : EBTNodeResult::InProgress;
}

void UBTTask_MoveToSkillTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return;


	const EPathFollowingStatus::Type Status = AIC->GetMoveStatus();

	if (Status == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (Status == EPathFollowingStatus::Waiting || Status == EPathFollowingStatus::Paused)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
