#include "MonsterAI/Task/Region/BTService_CheckCombatZoneExit.h"
#include "MonsterAI/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTService_CheckCombatZoneExit::UBTService_CheckCombatZoneExit()
{
	NodeName = TEXT("CheckCombatZoneExit");
	Interval = 1.0f;
}

void UBTService_CheckCombatZoneExit::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!Monster || !BB)
	{
		return;
	}

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
	if (!NavSystem)
	{
		return;
	}

	const FMonsterAttributeRow& Attribute = Monster->GetAttributeComponent()->GetMonsterAttribute();

	FNavLocation Dummy;
	const bool bMonsterOnNavMesh = NavSystem->ProjectPointToNavigation(Monster->GetActorLocation(), Dummy);
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
	const bool bTargetOnNavMesh = Target ? NavSystem->ProjectPointToNavigation(Target->GetActorLocation(), Dummy) : false;


	if (UMonsterStateComponent* StateComp = Monster->GetStateComponent())
	{
		if (StateComp->IsInState(EMonsterState::Combat) && (!bMonsterOnNavMesh || !bTargetOnNavMesh))
		{
			StateComp->SetState(EMonsterState::Return);
			UE_LOG(LogTemp, Warning, TEXT("[NavMeshExit] Monster left NavMesh, returning."));
		}
	}

}
