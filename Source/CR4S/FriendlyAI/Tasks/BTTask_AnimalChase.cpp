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
	
	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetActor))
	{
		if (TargetAnimal->RowName == Animal->RowName || TargetAnimal->CurrentState == EAnimalState::Dead)
		{

			Animal->ClearTarget();
			if (UBlackboardComponent* BB = Controller->GetBlackboardComponent())
			{
				BB->ClearValue(TEXT("TargetActor"));
			}

			Animal->SetAnimalState(EAnimalState::Patrol);

			return EBTNodeResult::Failed;
		}
	}
	
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

	if (!IsValid(TargetToChase) || (Cast<ABaseAnimal>(TargetToChase) && Cast<ABaseAnimal>(TargetToChase)->CurrentState == EAnimalState::Dead))
	{
		if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("TargetActor"));
		}
		Animal->ClearTarget();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
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