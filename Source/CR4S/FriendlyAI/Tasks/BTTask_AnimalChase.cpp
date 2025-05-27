#include "BTTask_AnimalChase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "../BaseAnimal.h"
#include "../Controller/AnimalAIController.h"

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

	AActor* TargetActor = Cast<AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;
    
	if (ABaseAnimal* TargetAnimal = Cast<ABaseAnimal>(TargetActor))
	{
		// …
		if (AAnimalAIController* C = Cast<AAnimalAIController>(OwnerComp.GetAIOwner()))
		{
			C->SetAnimalState(EAnimalState::Patrol);
		}
		return EBTNodeResult::Failed;
	}
    
	TargetToChase = TargetActor;

	const FVector SelfLocation   = Animal->GetActorLocation();
	const FVector TargetLocation = TargetToChase->GetActorLocation();
	const float Distance         = FVector::Dist(SelfLocation, TargetLocation);

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
		if (AAnimalAIController* C = Cast<AAnimalAIController>(OwnerComp.GetAIOwner()))
		{
			C->SetAnimalState(EAnimalState::Patrol);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	static FVector LastTargetLocation = FVector::ZeroVector;
	const FVector CurrentTargetLocation = TargetToChase->GetActorLocation();
	const float DistanceMoved = FVector::DistSquared(LastTargetLocation, CurrentTargetLocation);

	if (DistanceMoved > FMath::Square(50.f))
	{
		LastTargetLocation = CurrentTargetLocation;

		FAIMoveRequest MoveReq;
		MoveReq.SetGoalLocation(CurrentTargetLocation);
		MoveReq.SetAcceptanceRadius(100.f);

		if (AAIController* AIController = Cast<AAIController>(Animal->GetController()))
		{
			AIController->MoveTo(MoveReq);
		}
	}
}
