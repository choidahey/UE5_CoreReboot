#include "MonsterAI/Controller/SeasonBossMonsterAIController.h"
#include "BrainComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterStateComponent.h"

ASeasonBossMonsterAIController::ASeasonBossMonsterAIController()
{
}

void ASeasonBossMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (auto* StateComp = InPawn->FindComponentByClass<UMonsterStateComponent>())
	{
		StateComp->OnStunStarted.AddDynamic(this, &ASeasonBossMonsterAIController::HandleStunStarted);
		StateComp->OnStunEnded.AddDynamic(this, &ASeasonBossMonsterAIController::HandleStunEnded);
	}
}

void ASeasonBossMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ASeasonBossMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!IsValid(BlackboardComp)) return;

	if (auto* StateComp = GetPawn()->FindComponentByClass<UMonsterStateComponent>())
	{
		if (StateComp->IsStunned())
			return;
	}

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
	if (BlackboardComp->IsValidKey(BlackboardComp->GetKeyID(FSeasonBossAIKeys::bIsDashing)))
	{
		isDashing = BlackboardComp->GetValueAsBool(FSeasonBossAIKeys::bIsDashing);
	}
	
	if (IsValid(Target) && !isDashing)
	{
		LastFocusedTarget = Target;
		SetFocus(Target, EAIFocusPriority::Gameplay);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void ASeasonBossMonsterAIController::HandleStunStarted()
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("Stunned"));  
	}

	ClearFocus(EAIFocusPriority::Gameplay);
}

void ASeasonBossMonsterAIController::HandleStunEnded()
{
	if (BrainComponent)
	{
		BrainComponent->RestartLogic();  
	}

	if (AActor* Restore = LastFocusedTarget.Get())
	{
		SetFocus(Restore, EAIFocusPriority::Gameplay);
	}
}
