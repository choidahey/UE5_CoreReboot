#include "MonsterAI/Controller/SeasonBossMonsterAIController.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"

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
	
	if (!IsValid(BlackboardComp)) return;

	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn)) return;
	
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor));

	if (!IsValid(Target))
	{
		Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	}
	if (!IsValid(Target))
	{
		Target = UGameplayStatics::GetPlayerPawn(ControlledPawn->GetWorld(), 0);
	}

	bool isDashing = false;
	if (BlackboardComp->IsValidKey(BlackboardComp->GetKeyID(TEXT("IsDashing"))))
	{
		isDashing = BlackboardComp->GetValueAsBool(TEXT("IsDashing"));
	}
	
	if (IsValid(Target) && !isDashing)
	{
		SetFocus(Target, EAIFocusPriority::Gameplay);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}
