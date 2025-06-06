#include "MonsterAI/Controller/RegionBossMonsterAIController.h"
#include "MonsterAI/Region/RegionBossMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
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
		if (UMonsterStateComponent* StateComp = RegionBoss->FindComponentByClass<UMonsterStateComponent>())
		{
			StateComp->SetState(EMonsterState::Patrol);
		}

		if (RegionBoss->BehaviorTree)
		{
			UBlackboardComponent* RawBBComp = BlackboardComp.Get();
			UseBlackboard(RegionBoss->BehaviorTree->BlackboardAsset, RawBBComp);
			RunBehaviorTree(RegionBoss->BehaviorTree);
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
