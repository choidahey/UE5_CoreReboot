#include "MonsterAI/Controller/RegionBossMonsterAIController.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Components/MonsterStateComponent.h"

ARegionBossMonsterAIController::ARegionBossMonsterAIController()
	: MyHeader(TEXT("RegionBossAIC"))
{
}

void ARegionBossMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ARegionBossMonster* RegionBoss = Cast<ARegionBossMonster>(InPawn))
	{
		if (Blackboard)
		{
			Blackboard->SetValueAsVector(FRegionBossAIKeys::BaseCampLocation, RegionBoss->GetBaseCampLocation());
			Blackboard->SetValueAsInt(FRegionBossAIKeys::CurrentState, static_cast<int32>(EMonsterState::Idle));
		}
	}
}
