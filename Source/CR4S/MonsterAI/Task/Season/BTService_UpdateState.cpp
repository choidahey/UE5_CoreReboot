#include "MonsterAI/Task/Season/BTService_UpdateState.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_UpdateState::UBTService_UpdateState()
{
	NodeName = TEXT("UpdateState");
	Interval = 1.f;
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true;
	bCreateNodeInstance = true;
}

void UBTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!BB || !Pawn) return;

	bool bIsPlayingAttackMontage = BB->GetValueAsBool(FAIKeys::bIsPlayingAttackMontage);
	if (bIsPlayingAttackMontage) return;

	const FVector PawnLoc = Pawn->GetActorLocation();

	AActor* TargetPlayer = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	AActor* TargetHouse = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));

	float DistanceToPlayer = TNumericLimits<float>::Max();
	bool  bPlayerInRange = false;

	float DistanceToHouse = TNumericLimits<float>::Max();
	bool  bHouseInRange = false;

	if (TargetPlayer)
	{
		DistanceToPlayer = FVector::Dist(PawnLoc, TargetPlayer->GetActorLocation());
		bPlayerInRange = (DistanceToPlayer <= AttackDistanceThreshold);
	}

	if (TargetHouse)
	{
		DistanceToHouse = FVector::Dist(PawnLoc, TargetHouse->GetActorLocation());
		bHouseInRange = (DistanceToHouse <= AttackDistanceThreshold);
	}

	if (UMonsterStateComponent* StateComp = Pawn->FindComponentByClass<UMonsterStateComponent>())
	{
		if (IsValid(TargetPlayer))
		{
			if (bPlayerInRange)
				StateComp->SetState(EMonsterState::Attack);
			else
				StateComp->SetState(EMonsterState::Chase);
		}
		else if (IsValid(TargetHouse))
		{
			if (bHouseInRange)
			{
				StateComp->SetState(EMonsterState::AttackHouse);
			}
			else
			{
				StateComp->SetState(EMonsterState::MoveToHouse);
			}
		}
		else
		{
			StateComp->SetState(EMonsterState::Chase);
		}
	}
}
