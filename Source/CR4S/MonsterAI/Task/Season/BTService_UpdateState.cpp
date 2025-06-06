#include "MonsterAI/Task/Season/BTService_UpdateState.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CR4S.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_UpdateState::UBTService_UpdateState()
{
	NodeName = TEXT("UpdateState");
	Interval = 1.f;
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;  
	bCreateNodeInstance = true;
}

void UBTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return;

	bool bIsPlayingAttackMontage = BB->GetValueAsBool(FAIKeys::bIsPlayingAttackMontage);
	if (bIsPlayingAttackMontage) return;

	const FVector PawnLoc = Pawn->GetActorLocation();

	AActor* TargetPlayer = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	AActor* TargetHouse = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));

	float DistanceToPlayer = TNumericLimits<float>::Max();
	if (IsValid(TargetPlayer))
	{
		DistanceToPlayer = FVector::Dist(PawnLoc, TargetPlayer->GetActorLocation());
		bIsPlayerInAttackRange = (DistanceToPlayer <= AttackDistanceThreshold);
	}
	
	float DistanceToHouse = TNumericLimits<float>::Max();
	if (IsValid(TargetHouse))
	{
		DistanceToHouse = FVector::Dist(PawnLoc, TargetHouse->GetActorLocation());
		bIsHouseInAttackRange = (DistanceToHouse <= AttackDistanceThreshold);
	}

	if (UMonsterStateComponent* StateComp = Pawn->FindComponentByClass<UMonsterStateComponent>())
	{
		if (IsValid(TargetPlayer))
		{
			if (bIsPlayerInAttackRange)
			{
				StateComp->SetState(EMonsterState::Attack);
				BB->SetValueAsInt(FSeasonBossAIKeys::CurrentState, (int)EMonsterState::Attack);
			}
			else
			{
				StateComp->SetState(EMonsterState::Chase);
				BB->SetValueAsInt(FSeasonBossAIKeys::CurrentState, (int)EMonsterState::Chase);
			}
				
		}
		else if (IsValid(TargetHouse))
		{
			if (bIsHouseInAttackRange)
			{
				StateComp->SetState(EMonsterState::AttackHouse);
				BB->SetValueAsInt(FSeasonBossAIKeys::CurrentState, (int)EMonsterState::AttackHouse);
			}
			else
			{
				StateComp->SetState(EMonsterState::MoveToHouse);
				BB->SetValueAsInt(FSeasonBossAIKeys::CurrentState, (int)EMonsterState::MoveToHouse);
			}
		}
		else
		{
			StateComp->SetState(EMonsterState::Chase);
			BB->SetValueAsInt(FSeasonBossAIKeys::CurrentState, (int)EMonsterState::Chase);
		}

		CR4S_Log(LogDa, Log, TEXT("[UpdateState] Current State : %d"), BB->GetValueAsInt(FSeasonBossAIKeys::CurrentState));
	}
}
