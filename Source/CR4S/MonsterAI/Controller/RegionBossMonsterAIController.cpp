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
		if (UMonsterStateComponent* StateComp = RegionBoss->GetStateComponent())
		{
			StateComp->SetState(EMonsterState::Idle);
		}

		if (Blackboard)
		{
			RegionBoss->SetBaseCampLocation(RegionBoss->GetActorLocation());
			Blackboard->SetValueAsVector(FRegionBossAIKeys::BaseCampLocation, RegionBoss->GetBaseCampLocation());
		}
	}
}

void ARegionBossMonsterAIController::OnTargetDetected(AActor* DetectedActor)
{
	Super::OnTargetDetected(DetectedActor);

}

void ARegionBossMonsterAIController::OnTargetLost(AActor* LostActor)
{
}
