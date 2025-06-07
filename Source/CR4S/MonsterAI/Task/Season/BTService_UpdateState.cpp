#include "MonsterAI/Task/Season/BTService_UpdateState.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CR4S.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
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

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	APawn* OwnerPawn = AIC->GetPawn();
	if (!OwnerPawn) return;

	UMonsterAnimComponent* AnimComp = OwnerPawn->FindComponentByClass<UMonsterAnimComponent>();
	if (!IsValid(AnimComp)) return;

	const bool bIsAnyPlayingMontage = AnimComp->IsAnyMontagePlaying();
	if (bIsAnyPlayingMontage) return;

	const FVector PawnLoc = OwnerPawn->GetActorLocation();

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

		CR4S_Log(LogDa, Log,
			TEXT("[UpdateState] DistanceToHouse = %f, Threshold = %f, bIsHouseInAttackRange = %s"),
			DistanceToHouse,
			AttackDistanceThreshold,
			bIsHouseInAttackRange ? TEXT("True") : TEXT("False"));
	}
	else
	{
		CR4S_Log(LogDa, Log, TEXT("[UpdateState] TargetHouse is invalid or nullptr"));
	}

	if (UMonsterStateComponent* StateComp = OwnerPawn->FindComponentByClass<UMonsterStateComponent>())
	{
		if (IsValid(TargetPlayer))
		{
			if (bIsPlayerInAttackRange)
			{
				StateComp->SetState(EMonsterState::Attack);
			}
			else
			{
				StateComp->SetState(EMonsterState::Chase);
			}
				
		}
		else if (IsValid(TargetHouse))
		{
			if (bIsHouseInAttackRange)
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

		CR4S_Log(LogDa, Log, TEXT("[UpdateState] Current State : %d"), BB->GetValueAsInt(FSeasonBossAIKeys::CurrentState));
	}
}
