#include "MonsterAI/Task/Region/BTService_CheckCombatZoneExit.h"
#include "MonsterAI/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "NavigationSystem.h"
#include "MonsterAI/MonsterAIHelper.h"

void UBTService_CheckCombatZoneExit::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!Monster || !BlackboardComp) return;
	
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor));
	if (!Target) return;

	if (UMonsterStateComponent* StateComp = Monster->FindComponentByClass<UMonsterStateComponent>())
	{
		if (!StateComp->IsInState(EMonsterState::Combat)) return;
			
		const FVector PlayerLocation = Target->GetActorLocation();

		FNavLocation Dummy;
		const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Monster->GetWorld());
		if (!NavSystem) return;

		if (!NavSystem->ProjectPointToNavigation(PlayerLocation, Dummy, FVector(50, 50, 100)))
		{
			StateComp->SetState(EMonsterState::Return);
			BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, false);

			UE_LOG(LogTemp, Warning, TEXT("[NavMeshExit] Player left NavMesh, returning."));
		}
	}
}
