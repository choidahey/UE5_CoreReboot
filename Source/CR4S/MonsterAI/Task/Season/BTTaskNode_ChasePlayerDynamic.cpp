#include "MonsterAI/Task/Season/BTTaskNode_ChasePlayerDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Pawn.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTTaskNode_ChasePlayerDynamic::UBTTaskNode_ChasePlayerDynamic()
	: AcceptanceRadius(100.f)
{
	NodeName = TEXT("ChasePlayerDynamic");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_ChasePlayerDynamic::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!AIC || !TargetPlayer) return EBTNodeResult::Failed;

	ABaseMonster* MonsterCharacter = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UMonsterStateComponent* StateComp = MonsterCharacter->GetStateComponent();

	StateComp->SetState(EMonsterState::Chase);
	MoveRequestID = AIC->MoveToActor(TargetPlayer, AcceptanceRadius, false);

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_ChasePlayerDynamic::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* SelfPawn = AIC ? AIC->GetPawn() : nullptr;
	APawn* TargetPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!AIC || !TargetPlayerPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float Distance = FVector::DistSquared(SelfPawn->GetActorLocation(), TargetPlayerPawn->GetActorLocation());
	if (Distance <= FMath::Square(AcceptanceRadius))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	AIC->MoveToActor(TargetPlayerPawn, AcceptanceRadius, false);
}
