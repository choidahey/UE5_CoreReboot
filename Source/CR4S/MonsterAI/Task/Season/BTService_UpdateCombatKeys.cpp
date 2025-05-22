#include "MonsterAI/Task/Season/BTService_UpdateCombatKeys.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_UpdateCombatKeys::UBTService_UpdateCombatKeys()
{
	NodeName = TEXT("UpdateCombatKeys");
	Interval = 0.5f;
	bNotifyBecomeRelevant = true;
}

void UBTService_UpdateCombatKeys::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!BB || !Pawn) return;

	float Distance = TNumericLimits<float>::Max();
	bool  bInAttackRange = false;
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));

	if (Target)
	{
		Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

		bInAttackRange = Distance <= AttackDistanceThreshold;
	}

	BB->SetValueAsFloat(CurrentDistance.SelectedKeyName, Distance);
	BB->SetValueAsBool(bIsAttackRange.SelectedKeyName, bInAttackRange);
	UE_LOG(LogTemp, Log, TEXT("[UpdateCombatKeys] CurrentDistance %2.f"), Distance);

	if (auto* StateComp = Pawn->FindComponentByClass<UMonsterStateComponent>())
	{
		if (!Target)
		{
			StateComp->SetState(EMonsterState::Idle);
		}
		else if (bInAttackRange)
		{
			StateComp->SetState(EMonsterState::Attack);
		}
		else
		{
			StateComp->SetState(EMonsterState::Chase);
		}
	}
}
