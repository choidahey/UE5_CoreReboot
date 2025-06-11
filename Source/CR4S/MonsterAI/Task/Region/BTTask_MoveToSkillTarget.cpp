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

	SkillTargetLocationKey.SelectedKeyName = FRegionBossAIKeys::SkillTargetLocation;
	ApproachTypeKey.SelectedKeyName = FRegionBossAIKeys::ApproachType;
}

EBTNodeResult::Type UBTTask_MoveToSkillTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!IsValid(Monster)) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	TargetLocation = BlackboardComp->GetValueAsVector(SkillTargetLocationKey.SelectedKeyName);
	ApproachType = static_cast<EApproachType>(BlackboardComp->GetValueAsEnum(ApproachTypeKey.SelectedKeyName));

	UCharacterMovementComponent* MoveComp = Monster->FindComponentByClass<UCharacterMovementComponent>();
	if (!MoveComp) return EBTNodeResult::Failed;

	switch (ApproachType)
	{
	case EApproachType::Stand:
		return EBTNodeResult::Succeeded;

	case EApproachType::AdvanceCast:
	case EApproachType::Stalk:
		MoveComp->MaxWalkSpeed = SlowMoveSpeed;
		break;

	case EApproachType::Dash:
	case EApproachType::CloseBurst:
		MoveComp->MaxWalkSpeed = DashMoveSpeed;
		break;

	case EApproachType::Flank:
	{
		const FVector Forward = Monster->GetActorForwardVector();
		const FVector Right = Monster->GetActorRightVector();
		const FVector FlankOffset = Right * 300.f;
		const FVector FlankLocation = TargetLocation + FlankOffset;

		OwnerComp.GetAIOwner()->MoveToLocation(FlankLocation, AcceptableRadius);
		return EBTNodeResult::Succeeded;
	}

	case EApproachType::Teleport:
		Monster->SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
		return EBTNodeResult::Succeeded;

	default:
		break;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
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
