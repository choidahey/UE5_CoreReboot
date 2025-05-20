#include "BTTask_AnimalChase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "../BaseAnimal.h"

UBTTask_AnimalChase::UBTTask_AnimalChase()
{
	NodeName = TEXT("AnimalChase");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AnimalChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	ABaseAnimal* Animal = Cast<ABaseAnimal>(Controller->GetPawn());
	if (!Animal) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	TargetToChase = TargetActor;

	const FVector TargetLocation = TargetActor->GetActorLocation();

	Animal->SetAnimalState(EAnimalState::Chase);
	
	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(TargetLocation);
	MoveReq.SetAcceptanceRadius(100.f);

	if (AAIController* AIController = Cast<AAIController>(Animal->GetController()))
	{
		AIController->MoveTo(MoveReq);
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTask_AnimalChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{	
	ABaseAnimal* Animal = Cast<ABaseAnimal>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Animal || Animal->CurrentState != EAnimalState::Chase)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

		
	const FVector SelfLocation = Animal->GetActorLocation();
	const FVector TargetLocation = TargetToChase->GetActorLocation();

	const float Distance = FVector::Dist(SelfLocation, TargetLocation);

	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(TargetToChase->GetActorLocation());
	MoveReq.SetAcceptanceRadius(100.f);

	if (AAIController* AIController = Cast<AAIController>(Animal->GetController()))
	{
		AIController->MoveTo(MoveReq);
	}

}