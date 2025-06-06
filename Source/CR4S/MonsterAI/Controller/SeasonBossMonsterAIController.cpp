#include "MonsterAI/Controller/SeasonBossMonsterAIController.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

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

	bool bIsPlayingAttackMontage = BlackboardComp->GetValueAsBool(FAIKeys::bIsPlayingAttackMontage);
	
	if (bIsPlayingAttackMontage)
		ClearFocus(EAIFocusPriority::Gameplay);
	else
	{
		// SetFocus(Target, EAIFocusPriority::Gameplay);
		if (Target)
			SetFocus(Target, EAIFocusPriority::Gameplay);
		else
			ClearFocus(EAIFocusPriority::Gameplay);
	}
}
