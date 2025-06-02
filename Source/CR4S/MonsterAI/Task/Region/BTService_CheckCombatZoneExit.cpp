#include "MonsterAI/Task/Region/BTService_CheckCombatZoneExit.h"
#include "MonsterAI/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "NavigationSystem.h"
#include "MonsterAI/MonsterAIHelper.h"

void UBTService_CheckCombatZoneExit::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!Monster || !BlackboardComp) return;
	
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor));
	if (!Target) return;

	UMonsterStateComponent* StateComp = Monster->FindComponentByClass<UMonsterStateComponent>();
	if (!StateComp || !StateComp->IsInState(EMonsterState::Combat)) return;

	const FVector TargetLocation = Target->GetActorLocation();

	if (const ARegionBossMonster* RegionBoss = Cast<ARegionBossMonster>(Monster))
	{
		if (RegionBoss->IsOutsideCombatRange())
		{
			StateComp->SetState(EMonsterState::Return);
			BlackboardComp->SetValueAsBool(FAIKeys::CanSeePlayer, false);

			UE_LOG(LogTemp, Warning, TEXT("[CombatZoneExit] Boss exceeded combat range. Returning."));
		}
	}
}
