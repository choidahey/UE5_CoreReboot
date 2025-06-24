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

	APawn* OwnerPawn = GetPawn();
	if (!IsValid(OwnerPawn)) return;
	
	if (!IsValid(BlackboardComp)) return;
	
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FAIKeys::TargetActor));

	if (!IsValid(Target))
	{
		Target = Cast<AActor>(BlackboardComp->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	}
	if (!IsValid(Target))
	{
		Target = UGameplayStatics::GetPlayerPawn(OwnerPawn->GetWorld(), 0);
	}
	
	ABaseMonster* OwnerMonster = Cast<ABaseMonster>(OwnerPawn);
	if (!IsValid(OwnerMonster)) return;

	UMonsterAnimComponent* AnimComp = OwnerMonster->FindComponentByClass<UMonsterAnimComponent>();
	const bool bIsPlayingAttackMontage = (AnimComp != nullptr) ? AnimComp->IsAnyMontagePlaying() : false;

	if (Target)
	{
		SetFocus(Target, EAIFocusPriority::Gameplay);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}
