#include "MonsterAI/Task/Season/BTService_UpdateState.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "CR4S.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_UpdateState::UBTService_UpdateState()
{
	NodeName = TEXT("UpdateState");
	Interval = 0.5f;
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;  
	bCreateNodeInstance = true;

	TargetActorKey.SelectedKeyName = FAIKeys::TargetActor;
	NearestHouseActorKey.SelectedKeyName = FSeasonBossAIKeys::NearestHouseActor;
}

void UBTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	APawn* OwnerPawn = AIC->GetPawn();
	if (!OwnerPawn) return;

	UMonsterAnimComponent* AnimComp = OwnerPawn->FindComponentByClass<UMonsterAnimComponent>();
	if (!IsValid(AnimComp)) return;

	UMonsterStateComponent* StateComp = OwnerPawn->FindComponentByClass<UMonsterStateComponent>();
	if (!IsValid(StateComp)) return;

	UMonsterAttributeComponent* AttrComp = OwnerPawn->FindComponentByClass<UMonsterAttributeComponent>();
	if (!IsValid(AttrComp)) return;
	
	if (AnimComp->IsAnyMontagePlaying()) return;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	AActor* TargetHouse  = Cast<AActor>(BB->GetValueAsObject(NearestHouseActorKey.SelectedKeyName));

	const FVector PawnLoc = OwnerPawn->GetActorLocation();

	if (IsValid(TargetHouse))
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, TargetHouse);
		StateComp->SetState(EMonsterState::AttackHouse);
	}
	else if (IsValid(TargetActor))
	{
		StateComp->SetState(EMonsterState::Attack);
	}
	else
	{
		if (AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
			BB->SetValueAsObject(FAIKeys::TargetActor, PlayerPawn);
		
		StateComp->SetState(EMonsterState::Attack);
	}
	
	CR4S_Log(LogDa, Log, TEXT("[%s] %s Current State : %d"),
		*GetClass()->GetName(),
		*OwnerPawn->GetName(),
		BB->GetValueAsInt(FSeasonBossAIKeys::CurrentState));
}
