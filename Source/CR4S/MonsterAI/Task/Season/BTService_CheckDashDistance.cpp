#include "MonsterAI/Task/Season/BTService_CheckDashDistance.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckDashDistance::UBTService_CheckDashDistance()
{
	NodeName = TEXT("CheckDashDistance");
	Interval = 0.5f;
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

void UBTService_CheckDashDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!IsValid(AIC)) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BB)) return;

	APawn* Pawn = AIC->GetPawn();
	if (!IsValid(Pawn)) return;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));
	
	if (!IsValid(TargetActor))
	{
		BB->SetValueAsBool(bIsDashAwayKey.SelectedKeyName, false);
		BB->SetValueAsBool(bIsDashForwardKey.SelectedKeyName, false);
		return;
	}

	const float Dist = FVector::Dist(Pawn->GetActorLocation(), TargetActor->GetActorLocation());

	BB->SetValueAsBool(bIsDashAwayKey.SelectedKeyName, Dist <= AwayDistance);
	BB->SetValueAsBool(bIsDashForwardKey.SelectedKeyName, Dist >= ForwardDistance);
}
