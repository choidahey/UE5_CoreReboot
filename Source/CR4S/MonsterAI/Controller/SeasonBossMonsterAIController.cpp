#include "MonsterAI/Controller/SeasonBossMonsterAIController.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Character/Characters/PlayerCharacter.h"

ASeasonBossMonsterAIController::ASeasonBossMonsterAIController()
	: MyHeader(TEXT("SeasonBossAIController"))
{
}

void ASeasonBossMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASeasonBossMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASeasonBossMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor));
	if (!Target)
		Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));

	if (Target)
		SetFocus(Target, EAIFocusPriority::Gameplay);
	else
		ClearFocus(EAIFocusPriority::Gameplay);
}
