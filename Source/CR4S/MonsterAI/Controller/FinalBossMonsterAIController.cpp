#include "MonsterAI/Controller/FinalBossMonsterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

AFinalBossMonsterAIController::AFinalBossMonsterAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFinalBossMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AFinalBossMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AFinalBossMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(FAIKeys::TargetActor, Player);
	}
}
