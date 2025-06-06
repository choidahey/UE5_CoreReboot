#include "MonsterAI/Controller/RegionBossMonsterAIController.h"

ARegionBossMonsterAIController::ARegionBossMonsterAIController()
	: MyHeader(TEXT("RegionBossAIC"))
{
}

void ARegionBossMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void ARegionBossMonsterAIController::OnTargetDetected(AActor* DetectedActor)
{
	Super::OnTargetDetected(DetectedActor);

}

void ARegionBossMonsterAIController::OnTargetLost(AActor* LostActor)
{
}
